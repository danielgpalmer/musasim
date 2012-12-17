#include <stdint.h>
#include <stdbool.h>

#include "board.h"
#include "../logging.h"
#include "../musashi/m68kcpu.h"
#include "../musashi/m68k.h"
#include "../utils.h"

#ifdef GDBSERVER
#include "../gdbserver.h"
#endif

#define CHECKMASK 0xFFFF0000 //
static char TAG[] = "board";
static unsigned int currentfc;

/*
 *
 *
 *
 */

static const card* slots[NUM_SLOTS];
static bool interruptswaiting[NUM_SLOTS];
static bool busrequestwaiting[NUM_SLOTS];

#define NOCARD 0xFF

#define GETPPC m68k_get_reg(NULL, M68K_REG_PPC)
#define GETPC m68k_get_reg(NULL, M68K_REG_PC)

/**
 *
 */

#define SLOTADDRESSMASK 0xE00000
#define SLOTSHIFT 21

static inline uint8_t board_decode_slot(uint32_t address) {
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
	if (card->init != NULL ) {
		(card->init)();
	}
}

void board_tick(int cyclesexecuted) {
	for (int i = 0; i < NUM_SLOTS; i++) {
		if (slots[i] != NULL ) {
			if (slots[i]->tick != NULL ) {
				(slots[i]->tick)(cyclesexecuted);
			}
		}
	}
}

void board_poweroff() {
	for (int i = 0; i < NUM_SLOTS; i++) {
		if (slots[i] != NULL ) {
			if (slots[i]->dispose != NULL ) {
				(slots[i]->dispose)();
			}
		}
	}
}

void board_pause(bool paused) {
	for (int i = 0; i < NUM_SLOTS; i++) {
		if (slots[i] != NULL ) {
			if (slots[i]->pause != NULL ) {
				(slots[i]->pause)(paused);
			}
		}
	}
}

static inline uint8_t board_which_slot(const card* card) {
	for (int i = 0; i < SIZEOFARRAY(slots); i++) {
		if (slots[i] == card) {
			return i;
		}
	}
	return NOCARD;
}

// Bus mastering stuff

static bool buslocked = false;

void board_lock_bus(const card* card) {
	// The real board will have an arbiter that decides which bus request to forward to the CPU
	// and route the result back to that card.

	// TODO simulate a single bus req to the cpu
	// TODO lock cpu off of the bus.. keep ticks happening but make sure the time the CPU cant touch the bus is simulated

	busrequestwaiting[board_which_slot(card)] = true;
	buslocked = true;
	m68k_end_timeslice();
	(card->busreqack)();
}

void board_unlock_bus(const card* card) {
	busrequestwaiting[board_which_slot(card)] = false;
	buslocked = false;
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

static int curslot = 0; // the slot that is driving atm

static inline bool board_interrupt_sanitycheck(int slot) {
	if (slot != NOCARD && slot != 0 && slot != 7) {
		return true;
	}

	printf("Interrupt from card not in slot, or card in zero or seven\n");
	return false;
}

void board_raise_interrupt(const card* card) {

	int slot = board_which_slot(card);
	if (!board_interrupt_sanitycheck(slot)) {
		return;
	}

	// The current driver is requesting interrupt again?
	if (curslot == slot) {
		log_println(LEVEL_DEBUG, TAG,
				"Slot %d tried to raise an interrupt while it's interrupt is apparently being serviced", slot);
		return;
	}

	// The current driver is requesting interrupt again?
	if (interruptswaiting[slot] || curslot == slot) {
		log_println(LEVEL_DEBUG, TAG,
				"Slot %d tried to raise an interrupt while it is already scheduled to have it's interrupt serviced. %d is being serviced.",
				slot, curslot);
		return;
	}

	// no IRQ is happening.. do it!
	if (curslot == 0) {
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

void board_lower_interrupt(const card* card) {

	int slot = board_which_slot(card);
	if (!board_interrupt_sanitycheck(slot)) {
		return;
	}

	interruptswaiting[slot] = false; // make sure the card is no longer queue

	// If this card is the current driver..
	if (curslot == slot) {
		//printf("board_lower_interrupt(%d)\n", slot);

		// check the queue for a waiting card
		int newslot = 0;
		for (int i = 6; i < 0; i--) {
			if (interruptswaiting[i] == true) {
				interruptswaiting[i] = false;
				newslot = i;
				break;
			}
		}

		// issue the new level if there was a card waiting, otherwise issue 0
		m68k_set_irq(newslot);
		curslot = newslot;
	}
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
static inline bool board_checkaccess(const card* card, uint32_t address, unsigned int fc, bool write) {

	address &= CHECKMASK;

	uint8_t memorytype = DEFAULTMEMORYTYPE;
	bool failed = false;

	if (card->memorytype != NULL )
		memorytype = card->memorytype(address);

	// trying to execute from non-executable memory
	if (!write && (fc == 2 || fc == 6) && !(memorytype & CARDMEMORYTYPE_EXECUTABLE)) {
		char buff[] = "this should be a disassembly";
		//m68k_disassemble(buff, GETPC, M68K_CPU_TYPE_68000);
		log_println(LEVEL_INFO, TAG, "Executing non-executable memory, PC[0x%08x], PPC[0x%08x], %s", GETPC, GETPPC,
				buff);
		failed = true;
	}

	// access to supervisor memory as user!
	if ((fc == 1 || fc == 2) && (memorytype & CARDMEMORYTYPE_SUPERVISOR)) {
		log_println(LEVEL_INFO, TAG, "Accessing supervisor memory as user PC[0x%08x], PPC[0x%08x]", GETPC, GETPPC);
		failed = true;
	}

	// address isn't writable and this is a write!!
	if (write && !(memorytype & CARDMEMORYTYPE_WRITABLE)) {
		log_println(LEVEL_INFO, TAG, "writing to readonly memory PC[0x%08x], PPC[0x%08x]", GETPC, GETPPC);
		failed = true;
	}

	if (failed)
#ifdef GDBSERVER
		gdb_break("sandbox violation");
#else
	sim_sandboxvoilated();
#endif
	return failed;
}

static inline unsigned int board_read(unsigned int address, bool skipchecks, const card* busmaster, int width) {

	uint8_t slot = board_decode_slot(address);
	uint32_t slotaddress = address & SLOT_ADDRESS_MASK;
	if (slot != NOCARD) {
		const card* card = slots[slot];
		if (!skipchecks)
			board_checkaccess(card, slotaddress, currentfc, false);
		switch (width) {
			case 8:
				if (card->read_byte != NULL ) {
					if (card->validaddress(slotaddress)) {
						return (card->read_byte)(slotaddress);
					}
				}
				break;
			case 16:
				if (card->read_word != NULL ) {
					if (card->validaddress(slotaddress)) {
						return (card->read_word)(slotaddress);
					}
				}
				break;
			case 32:
				if (card->read_long != NULL ) {
					if (card->validaddress(slotaddress)) {
						return (card->read_long)(slotaddress);
					}
				}
				break;
		}

		log_println(LEVEL_INFO, TAG, "slot %d doesn't support %d bit read, PC[0x%08x]", slot, width, GETPC);
#ifdef GDBSERVER
		gdb_break("unsupported read access");
#endif

	}
	return 0;

}

unsigned int board_read_byte_internal(unsigned int address, bool skipchecks, const card* busmaster) {
	return board_read(address, skipchecks, busmaster, 8);
}

unsigned int board_read_byte(unsigned int address) {
	return board_read_byte_internal(address, false, NULL );
}

unsigned int board_read_word_internal(unsigned int address, bool skipchecks, const card* busmaster) {
	return board_read(address, skipchecks, busmaster, 16);
}

unsigned int board_read_word(unsigned int address) {
	return board_read_word_internal(address, false, NULL );
}

unsigned int board_read_long_internal(unsigned int address, bool skipchecks, const card* busmaster) {
	return board_read(address, skipchecks, busmaster, 32);
}

unsigned int board_read_long(unsigned int address) {
	return board_read_long_internal(address, false, NULL );
}

static inline void board_write(unsigned int address, unsigned int value, bool skipchecks, const card* busmaster,
		int width) {
	uint8_t slot = board_decode_slot(address);
	uint32_t slotaddress = address & SLOT_ADDRESS_MASK;
	if (slot != NOCARD) {
		const card* card = slots[slot];
		board_checkaccess(card, slotaddress, currentfc, true);
		switch (width) {
			case 8:
				if (card->write_byte != NULL ) {
					if (card->validaddress(slotaddress)) {
						(card->write_byte)(slotaddress, value);
						return;
					}
				}
				break;
			case 16:
				if (card->write_word != NULL ) {
					if (card->validaddress(slotaddress)) {
						(card->write_word)(slotaddress, value);
						return;
					}
				}
				break;
			case 32:
				if (card->write_long != NULL ) {
					if (card->validaddress(slotaddress)) {
						(card->write_long)(slotaddress, value);
						return;
					}
				}
				break;
		}

		log_println(LEVEL_INFO, TAG, "slot %d doesn't support %d bit write", slot, width);
#ifdef GDBSERVER
		gdb_break("unsupported memory write");
#endif
	}
}

void board_write_byte_internal(unsigned int address, unsigned int value, bool skipchecks, const card* busmaster) {
	board_write(address, value, skipchecks, busmaster, 8);
}

void board_write_byte(unsigned int address, unsigned int value) {
	board_write_byte_internal(address, value, false, NULL );
}

void board_write_word_internal(unsigned int address, unsigned int value, bool skipchecks, const card* busmaster) {
	board_write(address, value, skipchecks, busmaster, 16);
}

void board_write_word(unsigned int address, unsigned int value) {
	board_write_word_internal(address, value, false, NULL );
}

void board_write_long_internal(unsigned int address, unsigned int value, bool skipchecks, const card* busmaster) {
	board_write(address, value, skipchecks, busmaster, 32);
}

void board_write_long(unsigned int address, unsigned int value) {
	board_write_long_internal(address, value, false, NULL );
}

void board_reset() {
	for (int i = 0; i < SIZEOFARRAY(slots); i++) {
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

	for (int i = 0; i < SIZEOFARRAY(slots); i++) {
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

	for (int i = 0; i < SIZEOFARRAY(slots); i++) {
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
