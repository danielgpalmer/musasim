#include <stdint.h>
#include <stdbool.h>

#include "board.h"
#include "../logging.h"
#include "../musashi/m68kcpu.h"
#include "../musashi/m68k.h"
#include "../utils.h"

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

#define GETPC m68k_get_reg(NULL, M68K_REG_PC)

/**
 *
 */

#define SLOTADDRESSMASK 0xE00000

static uint8_t board_decode_slot(uint32_t address) {

	uint8_t slot = (address & SLOTADDRESSMASK) >> 21;

	if (slots[slot] == NULL) {
		log_println(LEVEL_DEBUG, TAG, "Address decoded to slot %d but there is no card in that slot, PC[0x%08x]", slot,
				GETPC);
		return NOCARD;
	}

	return slot;

}

void board_add_device(uint8_t slot, const card *card) {

	log_println(LEVEL_DEBUG, TAG, "Inserting %s into slot %d", card->boardinfo, slot);

	slots[slot] = card;
	if (card->init != NULL) {
		(card->init)();
	}
}

void board_tick(int cyclesexecuted) {
	for (int i = 0; i < NUM_SLOTS; i++) {
		if (slots[i] != NULL) {
			if (slots[i]->tick != NULL) {
				(slots[i]->tick)(cyclesexecuted);
			}
		}
	}
}

void board_poweroff() {
	for (int i = 0; i < NUM_SLOTS; i++) {
		if (slots[i] != NULL) {
			if (slots[i]->dispose != NULL) {
				(slots[i]->dispose)();
			}
		}
	}
}

static uint8_t board_which_slot(const card* card) {

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

int curslot = 0; // the slot that is driving atm

static bool board_interrupt_sanitycheck(int slot) {
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
	}
	// Someone else is driving.. queue
	else {
		printf("queue %d\n", slot);
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

	if (slots[curslot]->irqack != NULL) {
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
static bool board_checkaccess(uint8_t slot, uint32_t address, unsigned int fc, bool write) {

	uint8_t memorytype = DEFAULTMEMORYTYPE;
	bool failed = false;

	if (slots[slot]->memorytype != NULL)
		memorytype = slots[slot]->memorytype(address);

	if ((fc == 1 || fc == 2) && (memorytype & CARDMEMORYTYPE_SUPERVISOR)) {
		// access to supervisor memory as user!
		failed = true;
	}

	if ((fc == 2 || fc == 6) && !(memorytype & CARDMEMORYTYPE_EXECUTABLE)) {
		// trying to execute from non-executable memory
		failed = true;
	}

	if (write && !(memorytype & CARDMEMORYTYPE_WRITABLE)) {
		// address isn't writable and this is a write!!
		failed = true;
	}

	return failed;

}

unsigned int board_read_byte(unsigned int address) {
	uint8_t slot = board_decode_slot(address);
	uint32_t slotaddress = address & SLOT_ADDRESS_MASK;
	if (slot != NOCARD) {
		if (slots[slot]->read_byte != NULL) {
			if (slots[slot]->validaddress(slotaddress)) {
				return (slots[slot]->read_byte)(slotaddress);
			}
		}
		else {
			log_println(LEVEL_INFO, TAG, "slot %d doesn't support byte read", slot);
		}
	}
	return 0;
}

unsigned int board_read_word(unsigned int address) {

	if (address % 2 != 0) {
		log_println(LEVEL_DEBUG, TAG, "Word reads must be aligned, read from 0x%08x PC[0x%08x]", address, GETPC);
		return 0;
	}

	uint8_t slot = board_decode_slot(address);
	uint32_t slotaddress = address & SLOT_ADDRESS_MASK;
	if (slot != NOCARD) {
		if (slots[slot]->read_word != NULL) {
			if (slots[slot]->validaddress(slotaddress)) {
				return (slots[slot]->read_word)(slotaddress);
			}
		}
		else {
			log_println(LEVEL_INFO, TAG, "slot %d doesn't support word read", slot);
		}
	}
	return 0;
}

unsigned int board_read_long(unsigned int address) {
	uint8_t slot = board_decode_slot(address);
	uint32_t slotaddress = address & SLOT_ADDRESS_MASK;
	if (slot != NOCARD) {

		board_checkaccess(slot, slotaddress, currentfc, false);

		if (slots[slot]->read_long != NULL) {
			if (slots[slot]->validaddress(slotaddress)) {
				return (slots[slot]->read_long)(slotaddress);
			}
		}
		else {
			log_println(LEVEL_INFO, TAG, "slot %d doesn't support long read, PC[0x%08x]", slot, GETPC);
		}
	}
	return 0;
}

void board_write_byte(unsigned int address, unsigned int value) {
	uint8_t slot = board_decode_slot(address);
	uint32_t slotaddress = address & SLOT_ADDRESS_MASK;
	if (slot != NOCARD) {
		if (slots[slot]->write_byte != NULL) {
			if (slots[slot]->validaddress(slotaddress)) {
				(slots[slot]->write_byte)(slotaddress, value);
			}
		}
		else {
			log_println(LEVEL_INFO, TAG, "slot %d doesn't support byte write", slot);
		}
	}
}

void board_write_word(unsigned int address, unsigned int value) {

	if (address % 2 != 0) {
		log_println(LEVEL_DEBUG, TAG, "Word writes must be aligned, PC[0x%08x]", GETPC);
		return;
	}

	uint8_t slot = board_decode_slot(address);
	uint32_t slotaddress = address & SLOT_ADDRESS_MASK;
	if (slot != NOCARD) {
		if (slots[slot]->write_word != NULL) {
			if (slots[slot]->validaddress(slotaddress)) {
				(slots[slot]->write_word)(slotaddress, value);
			}
		}
		else {
			log_println(LEVEL_INFO, TAG, "slot %d doesn't support word write, PC[0x%08x]", slot, GETPC);
		}
	}
}

void board_write_long(unsigned int address, unsigned int value) {
	uint8_t slot = board_decode_slot(address);
	uint32_t slotaddress = address & SLOT_ADDRESS_MASK;
	if (slot != NOCARD) {
		if (slots[slot]->write_long != NULL) {
			if (slots[slot]->validaddress(slotaddress)) {
				(slots[slot]->write_long)(slotaddress, value);
			}
		}
		else {
			log_println(LEVEL_INFO, TAG, "slot %d doesn't support long write,  PC[0x%08x]", slot, GETPC);
		}
	}
}

void board_reset() {
	for (int i = 0; i < SIZEOFARRAY(slots); i++) {
		const card* c = slots[i];
		if (c != NULL) {
			if (c->reset != NULL) {
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
		if (c != NULL) {
			if (c->cyclesleft != NULL) {
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
		if (c != NULL) {
			if (c->setfc != NULL) {
				c->setfc(fc);
			}
		}
	}
}

const card* board_getcardinslot(int slot) {
	return slots[slot];
}
