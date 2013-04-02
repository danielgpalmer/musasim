#include <stdint.h>
#include <inttypes.h>
#include <stdbool.h>
#include <glib.h>
#include <sys/sysinfo.h>

#include "board.h"
#include "board_private.h"
#include "../logging.h"
#include "../musashi/m68kcpu.h"
#include "../musashi/m68k.h"
#include "../utils.h"
#include "../config.h"

#define LOTSOFDEBUGOUTPUT
#define TAG "board"

#ifdef GDBSERVER
#include "../gdbserver.h"
#endif

#if USEMULTIPLETHREADS
//
static int unprocessed = 0;
static GCond cond;
static GMutex mutex;
//
#endif

#if USEMULTIPLETHREADS
static GThreadPool* workers;
static GRecMutex busmutex;
#endif

#define CHECKMASK 0xFFFFFFFC //
static unsigned int currentfc;

/*
 *
 *
 *
 */

static const card* slots[NUM_SLOTS];
static long executiontimes[NUM_SLOTS];
static bool interruptswaiting[NUM_SLOTS];
static bool busrequestwaiting[NUM_SLOTS];
static bool buslocked = false;
static int curbusmaster = -1;
static int curslot = 0; // the slot that is driving atm

#define NOCARD 0xFF

#define GETPPC m68k_get_reg(NULL, M68K_REG_PPC)
#define GETPC m68k_get_reg(NULL, M68K_REG_PC)
#define GETSR m68k_get_reg(NULL, M68K_REG_SR)

/**
 *
 */

#define SLOTADDRESSMASK 0xE00000
#define SLOTSHIFT 21

static const inline uint8_t board_decode_slot(uint32_t address) {
	uint8_t slot = (address & SLOTADDRESSMASK) >> SLOTSHIFT;
	if (slots[slot] == NULL ) {
		log_println(LEVEL_DEBUG, TAG, "Address decoded to slot %d but there is no card in that slot, PC[0x%08x]", slot,
				GETPC);
		return NOCARD;
	}
	return slot;
}

void board_add_device(uint8_t slot, const card *card) {
	log_println(LEVEL_DEBUG, TAG, "Inserting %s into slot %d", card->boardinfo, slot);
	slots[slot] = card;
	if (card->init != NULL )
		(card->init)();
	if (card->reset != NULL )
		(card->reset)();
}

static int cycles;
static bool isbehind;

static void board_workerfunc(gpointer data, gpointer userdata) {
	struct timespec start, end;
	int slot = GPOINTER_TO_INT(data) - 1;
	if (slots[slot] != NULL && slots[slot]->tick != NULL ) {
		clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &start);
		slots[slot]->tick(cycles, isbehind);
		clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &end);
		struct timespec* diff = timespecdiff(&start, &end);
		executiontimes[slot] = (diff->tv_sec * SIM_ONENANOSECOND) + diff->tv_nsec;
	}
#if USEMULTIPLETHREADS
	g_mutex_lock(&mutex);
	unprocessed--;
	if (unprocessed == 0)
	g_cond_signal(&cond);
	g_mutex_unlock(&mutex);
#endif
}
void board_tick(int cyclesexecuted, bool behind) __attribute__((hot));
void board_tick(int cyclesexecuted, bool behind) {
	cycles = cyclesexecuted;
	isbehind = behind;
#if USEMULTIPLETHREADS
	g_mutex_lock(&mutex);
	unprocessed = 0;
	for (int i = 0; i < NUM_SLOTS; i++) {
		g_thread_pool_push(workers, GINT_TO_POINTER(i + 1), NULL );
		unprocessed++;
	}

	g_cond_wait(&cond, &mutex);
	g_assert(unprocessed == 0);
	g_mutex_unlock(&mutex);
#else
	for (int i = 0; i < NUM_SLOTS; i++) {
		board_workerfunc(GINT_TO_POINTER(i + 1), NULL );
	}
#endif

	//for (int i = 0; i < G_N_ELEMENTS(executiontimes); i++) {
	//	log_println(LEVEL_INFO, TAG, "slot %d (%s) took %ld", i, slots[i]->boardinfo, executiontimes[i]);
	//}

}

void board_poweron() {
#if USEMULTIPLETHREADS
	//int processors = get_nprocs();
	int processors = 1;
	workers = g_thread_pool_new(board_workerfunc, NULL, processors, true, NULL );
	log_println(LEVEL_INFO, TAG, "Started %d worker threads", processors);
#endif
}

void board_poweroff() {
	for (int i = 0; i < NUM_SLOTS; i++) {
		if (slots[i] != NULL && slots[i]->dispose != NULL ) {
			(slots[i]->dispose)();
		}
	}
#if USEMULTIPLETHREADS
	g_thread_pool_free(workers, true, true);
	workers = NULL;
#endif
}

void board_pause(bool paused) {
	for (int i = 0; i < NUM_SLOTS; i++) {
		if (slots[i] != NULL && slots[i]->pause != NULL ) {
			(slots[i]->pause)(paused);
		}
	}
}

static inline uint8_t board_which_slot(const card* card) {
	if (card == NULL )
		return NOCARD;

	for (int i = 0; i < G_N_ELEMENTS(slots); i++) {
		if (slots[i] == card) {
			return i;
		}
	}
	return NOCARD;
}

// Bus mastering stuff

static void board_dobuslock(int slot) {
	buslocked = true;
	curbusmaster = slot;
	m68k_end_timeslice();
	(slots[slot]->busreqack)();
}

void board_lock_bus(const card* card) {
#if USEMULTIPLETHREADS
	g_rec_mutex_lock(&busmutex);
#endif
	int slot = board_which_slot(card);
	if (buslocked) {
		g_assert(curbusmaster != slot);
		// cards should not be trying to lock the bus when they've already locked it
		busrequestwaiting[slot] = true;
	}
	else
		board_dobuslock(slot);
#if USEMULTIPLETHREADS
	g_rec_mutex_unlock(&busmutex);
#endif
}

void board_unlock_bus(const card* card) {
#if USEMULTIPLETHREADS
	g_rec_mutex_lock(&busmutex);
#endif
	buslocked = false;
	curbusmaster = -1;
	for (int i = 0; i < NUM_SLOTS; i++)
		if (busrequestwaiting[i])
			board_dobuslock(i);
#if USEMULTIPLETHREADS
	g_rec_mutex_unlock(&busmutex);
#endif
}

bool board_bus_locked() {
	return buslocked;
}

/* Interrupts
 - Each slot has an interrupt line except two; slot 0 and slot 7
 A priority encoder takes the interrupt lines and issues the highest
 to the CPU.
 - If two or more interrupts happen at the same time or an interrupt fires
 while another is waiting to be ack'ed it will be held and the priority encoder
 will issue the in priority order

 - Each slot has an ack line that is pulled low once the CPU starts
 servicing it's interrupt
 */

static inline const bool board_interrupt_sanitycheck(int slot) {
	if (slot != NOCARD && slot != 0 && slot != 7)
		return true;
//printf("Interrupt from card not in slot, or card in zero or seven\n");
	else
		return false;
}

void board_raise_interrupt(const card* card) {
#if USEMULTIPLETHREADS
	g_rec_mutex_lock(&busmutex);
#endif

	int slot = board_which_slot(card);
	if (board_interrupt_sanitycheck(slot)) {
// The current driver is requesting interrupt again?
		if (curslot == slot) {
			log_println(LEVEL_DEBUG, TAG,
					"Slot %d tried to raise an interrupt while it's interrupt is apparently being serviced", slot);
		}

// The current driver is requesting interrupt again?
		else if (interruptswaiting[slot] || curslot == slot) {
			log_println(LEVEL_DEBUG, TAG,
					"Slot %d tried to raise an interrupt while it is already scheduled to have it's interrupt serviced. %d is being serviced.",
					slot, curslot);
		}

// no IRQ is happening.. do it!
		else if (curslot == 0) {
			//printf("board_raise_interrupt(%d) SR - 0x%x\n", slot, m68k_get_reg(NULL, M68K_REG_SR));
			curslot = board_which_slot(card);
			m68k_set_irq(slot);
#ifdef GDBSERVER
			gdbserver_enteringinterrupt(slot);
#endif
		}
// Someone else is driving.. queue
		else {
			interruptswaiting[slot] = true;
		}
	}
#if USEMULTIPLETHREADS
	g_rec_mutex_unlock(&busmutex);
#endif

}

void board_lower_interrupt(const card* card) {
#if USEMULTIPLETHREADS
	g_rec_mutex_lock(&busmutex);
#endif

	int slot = board_which_slot(card);
	if (board_interrupt_sanitycheck(slot)) {
		interruptswaiting[slot] = false; // make sure the card is no longer queue

// If this card is the current driver..
		if (curslot == slot) {
			// check the queue for a waiting card
			int newslot = 0;
			for (int i = 6; i < 0; i--) {
				if (interruptswaiting[i] == true) {
					interruptswaiting[i] = false;
					newslot = i;
					break;
				}
			}

			// issue the new level if there was a card waiting, otherwise issue 0 (no interrupt)
			m68k_set_irq(newslot);
			curslot = newslot;
		}
	}
#if USEMULTIPLETHREADS
	g_rec_mutex_unlock(&busmutex);
#endif
}

int board_ack_interrupt(int level) {
	log_println(LEVEL_INSANE, TAG, "board_ack_interrupt(%d)", level);

	if (slots[curslot]->irqack != NULL ) {
		(slots[curslot]->irqack)();
		return M68K_INT_ACK_AUTOVECTOR;
	}
	else {
		log_println(LEVEL_WARNING, TAG, "card in %d doesn't have interrupt ack", curslot);
		return M68K_INT_ACK_SPURIOUS;
	}
}

// this will be for checking if an access to an address is valid in the current
// mode etc.. not decided if this will just be for the gdb version or if this
// will go in hardware too.
// returns TRUE if the access passes checks

static void board_logaccessviolation(uint32_t address, const char* violationdescription, const card* busmaster) {
	if (busmaster != NULL )
		log_println(LEVEL_INFO, TAG, "violation @0x%"PRIx32"; %s by busmaster in slot %d (%s), SR[0x%04x] FC[0x%04x]",
				address, violationdescription, board_which_slot(busmaster), busmaster->boardinfo, GETSR, currentfc);
	else
		log_println(LEVEL_INFO, TAG, "violation @0x%"PRIx32"; %s, PC[0x%08x], PPC[0x%08x], SR[0x%04x] FC[0x%04x]",
				address, violationdescription, GETPC, GETPPC, GETSR, currentfc);
}

static bool board_checkaccess(const card* accessedcard, uint32_t address, unsigned int fc, bool write,
		const card* busmaster) {

//static bool cachevalid[0xFFFF];
//static bool cacheresult[0xFFFF];

//address &= CHECKMASK;
//int cacheindex = address >> 16;

//if (cachevalid[cacheindex])
//	return cacheresult[cacheindex];

	uint8_t memorytype = DEFAULTMEMORYTYPE;
	bool passed = true;

	if (accessedcard->memorytype != NULL )
		memorytype = accessedcard->memorytype(address);

// trying to execute from non-executable memory
	if ((fc == FCUSERPROGRAM || fc == FCSUPERVISORPROGRAM) && !(memorytype & CARDMEMORYTYPE_EXECUTABLE)) {
		board_logaccessviolation(address, "Executing non-executable memory", busmaster);
		passed = false;
	}

// access to supervisor memory as user!
	if ((fc == FCUSERDATA || fc == FCUSERPROGRAM) && (memorytype & CARDMEMORYTYPE_SUPERVISOR)) {
		board_logaccessviolation(address, "Accessing supervisor memory as user", busmaster);
		passed = false;
	}

// address isn't writable and this is a write!!
	if (!write && !(memorytype & CARDMEMORYTYPE_READABLE)) {
		board_logaccessviolation(address, "Reading from un-readable memory", busmaster);
		passed = false;
	}

// address isn't writable and this is a write!!
	if (write && !(memorytype & CARDMEMORYTYPE_WRITABLE)) {
		board_logaccessviolation(address, "Writing to un-writable memory", busmaster);
		passed = false;
	}

	if (!passed) {
		busmaster->abort();
#ifdef GDBSERVER
		gdb_break("sandbox violation");
#else
		sim_sandboxviolated();
#endif
	}

//cachevalid[cacheindex] = true;
//cacheresult[cacheindex] = failed;

	return passed;
}

static inline unsigned int board_read(unsigned int address, bool skipchecks, const card* busmaster, const int width) __attribute__((always_inline)) __attribute__((hot));
static inline unsigned int board_read(unsigned int address, bool skipchecks, const card* busmaster, const int width) {

#if USEMULTIPLETHREADS
	g_rec_mutex_lock(&busmutex);
#endif

	uint8_t slot = board_decode_slot(address);
	uint32_t slotaddress = address & SLOT_ADDRESS_MASK;
	unsigned int value = 0;
	if (slot != NOCARD) {
		const card* card = slots[slot];
		if (skipchecks || board_checkaccess(card, slotaddress, currentfc, false, busmaster)) {
			if (card->validaddress(slotaddress)) {
				switch (width) {
					case 1:
						if (card->read_byte != NULL ) {
							value = (card->read_byte)(slotaddress);
							break;
						}
						goto READSIZENOTSUPPORTED;
					case 2:
						if (card->read_word != NULL ) {
							value = (card->read_word)(slotaddress);
							break;
						}
						goto READSIZENOTSUPPORTED;
					case 4:
						if (card->read_long != NULL ) {
							value = (card->read_long)(slotaddress);
							break;
						}
						READSIZENOTSUPPORTED: //
						default:
						log_println(LEVEL_INFO, TAG, "slot %d doesn't support %d byte read, PC[0x%08x]", slot, width,
								GETPC);
#ifdef GDBSERVER
						gdb_break("unsupported read access");
#endif
						break;

				}
			}
		}
	}
	else {
#ifdef GDBSERVER
		gdb_break("read from empty slot");
#endif
	}

#if USEMULTIPLETHREADS
	g_rec_mutex_unlock(&busmutex);
#endif

	return value;
}

unsigned int board_read_byte_internal(unsigned int address, bool skipchecks, const card* busmaster) {
	return board_read(address, skipchecks, busmaster, 1);
}

unsigned int board_read_byte_cpu(unsigned int address) {
	g_assert(!board_bus_locked());
	return board_read_byte_internal(address, false, NULL );
}

unsigned int board_read_byte_busmaster(unsigned int address, const card* busmaster) {
	g_assert(board_bus_locked());
	return board_read_byte_internal(address, false, busmaster);
}

unsigned int board_read_word_internal(unsigned int address, bool skipchecks, const card* busmaster) {
	return board_read(address, skipchecks, busmaster, 2);
}

unsigned int board_read_word_cpu(unsigned int address) {
	g_assert(!board_bus_locked());
	return board_read_word_internal(address, false, NULL );
}

unsigned int board_read_word_busmaster(unsigned int address, const card* busmaster) {
	g_assert(board_bus_locked());
	return board_read_word_internal(address, false, busmaster);
}

unsigned int board_read_long_internal(unsigned int address, bool skipchecks, const card* busmaster) {
	return board_read(address, skipchecks, busmaster, 4);
}

unsigned int board_read_long_cpu(unsigned int address) {
	g_assert(!board_bus_locked());
	return board_read_long_internal(address, false, NULL );
}

unsigned int board_read_long_busmaster(unsigned int address, const card* busmaster) {
	g_assert(board_bus_locked());
	return board_read_long_internal(address, false, busmaster);
}

static inline void board_write(unsigned int address, unsigned int value, bool skipchecks, const card* busmaster,
		const int width) __attribute__((always_inline)) __attribute__((hot));
static inline void board_write(unsigned int address, unsigned int value, bool skipchecks, const card* busmaster,
		const int width) {
#if USEMULTIPLETHREADS
	g_rec_mutex_lock(&busmutex);
#endif

	uint8_t slot = board_decode_slot(address);
	uint32_t slotaddress = address & SLOT_ADDRESS_MASK;
	if (slot != NOCARD) {
		const card* card = slots[slot];
		if (skipchecks || board_checkaccess(card, slotaddress, currentfc, true, busmaster)) {
			if (card->validaddress(slotaddress)) {
				switch (width) {
					case 1:
						if (card->write_byte != NULL ) {
							(card->write_byte)(slotaddress, value);
							break;
						}
						goto WRITESIZENOTSUPPORTED;
					case 2:
						if (card->write_word != NULL ) {
							(card->write_word)(slotaddress, value);
							break;
						}
						goto WRITESIZENOTSUPPORTED;
					case 4:
						if (card->write_long != NULL ) {
							(card->write_long)(slotaddress, value);
							break;
						}
						WRITESIZENOTSUPPORTED: //
						default:
						log_println(LEVEL_INFO, TAG, "slot %d doesn't support %d byte write", slot, width);
#ifdef GDBSERVER
						gdb_break("unsupported memory write");
#endif
						break;
				}
			}
		}
	}
	else {
#ifdef GDBSERVER
		gdb_break("write to empty slot");
#endif
	}

#if USEMULTIPLETHREADS
	g_rec_mutex_unlock(&busmutex);
#endif
}

void board_write_byte_internal(unsigned int address, unsigned int value, bool skipchecks, const card* busmaster) {
	board_write(address, value, skipchecks, busmaster, 1);
}

void board_write_byte_cpu(unsigned int address, unsigned int value) {
	g_assert(!board_bus_locked());
	board_write_byte_internal(address, value, false, NULL );
}

void board_write_byte_busmaster(unsigned int address, unsigned int value, const card* busmaster) {
	g_assert(board_bus_locked());
	board_write_byte_internal(address, value, false, busmaster);
}

void board_write_word_internal(unsigned int address, unsigned int value, bool skipchecks, const card* busmaster) {
	board_write(address, value, skipchecks, busmaster, 2);
}

void board_write_word_cpu(unsigned int address, unsigned int value) {
	g_assert(!board_bus_locked());
	board_write_word_internal(address, value, false, NULL );
}

void board_write_word_busmaster(unsigned int address, unsigned int value, const card* busmaster) {
	g_assert(board_bus_locked());
	board_write_word_internal(address, value, false, busmaster);
}

void board_write_long_internal(unsigned int address, unsigned int value, bool skipchecks, const card* busmaster) {
	board_write(address, value, skipchecks, busmaster, 4);
}

void board_write_long_cpu(unsigned int address, unsigned int value) {
	g_assert(!board_bus_locked());
	board_write_long_internal(address, value, false, NULL );
}

void board_write_long_busmaster(unsigned int address, unsigned int value, const card* busmaster) {
	g_assert(board_bus_locked());
	board_write_long_internal(address, value, false, busmaster);
}

void board_reset() {
	for (int i = 0; i < G_N_ELEMENTS(slots); i++) {
		const card* c = slots[i];
		if (c != NULL ) {
			if (c->reset != NULL ) {
				c->reset();
			}
		}
	}
}

int board_bestcasecycles() {
	return slots[SLOT_VIDEOCARD]->bestcasecycles();
}

int board_maxcycles(int numberofcyclesplanned) {
	int cycles = numberofcyclesplanned;
	for (int i = 0; i < G_N_ELEMENTS(slots); i++) {
		const card* c = slots[i];
		if (c != NULL ) {
			if (c->cyclesleft != NULL ) {
				int cardcycles = c->cyclesleft();
				if (cardcycles != -1 && cardcycles < cycles)
					cycles = cardcycles;
			}
		}
	}
	return cycles;
}

void board_setfc(unsigned int fc) {
	currentfc = fc;
	for (int i = 0; i < G_N_ELEMENTS(slots); i++) {
		const card* c = slots[i];
		if (c != NULL ) {
			if (c->setfc != NULL ) {
				c->setfc(fc);
			}
		}
	}
}

const card* board_getcardinslot(int slot) {
	return slots[slot];
}
