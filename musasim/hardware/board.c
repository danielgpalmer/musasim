#include <stdint.h>
#include <stdbool.h>

#include "stdio.h"
#include "board.h"
#include "../../m68kcpu.h"

/*
 *
 *
 *
 */

card* slots[NUM_SLOTS];
bool interruptswaiting[NUM_SLOTS];
bool busrequestwaiting[NUM_SLOTS];

#define NOCARD 0xFF

/**
 *
 */

#define SLOTADDRESSMASK 0xE00000

uint8_t board_decode_slot(uint32_t address) {

	uint8_t slot = (address & SLOTADDRESSMASK) >> 21;

	if (slots[slot] == NULL) {
		printf("*** NO CARD IN SLOT ***\n");
		return NOCARD;
	}

	return slot;

}

void board_add_device(uint8_t slot, card *card) {
	slots[slot] = card;
	if (card->init != NULL) {
		(card->init)();
	}
}

void board_tick() {
	for (int i = 0; i < NUM_SLOTS; i++) {
		if (slots[i] != NULL) {
			if (slots[i]->tick != NULL) {
				(slots[i]->tick)();
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

uint8_t board_which_slot(card* card) {

	for (int i = 0; i < sizeof(slots); i++) {
		if (slots[i] == card) {
			return i;
		}
	}

	return NOCARD;

}

// Bus mastering stuff

bool buslocked = false;

void board_lock_bus(card* card) {

	// The real board will have an arbiter that decides which bus request to forward to the CPU
	// and route the result back to that card.

	// TODO simulate a single bus req to the cpu
	// TODO lock cpu off of the bus.. keep ticks happening but make sure the time the CPU cant touch the bus is simulated

	busrequestwaiting[board_which_slot(card)] = true;
}

void board_unlock_bus() {
	busrequestwaiting[board_which_slot(card)] = false;
}

bool board_bus_locked() {
	return buslocked;
}

//

// Interrupts

// - Each slot has an interrupt line except one (not decided yet)
//   A priority encoder issues the interrupt to the CPU
//
// - Each slot has an ack line that is pulled low once the CPU starts servicing
//   this interrupt
//
// - If two or more interrupts happen at the same time or an interrupt fires
//   while another is waiting to be ack'ed it will be held and the priority encoder
//   will issue the in priority order

void board_raise_interrupt(card* card) {
	printf("raise interrupt\n");
	m68k_set_irq(5);
}

void board_lower_interrupt(card* card) {
	m68k_set_irq(0);
}

int board_ack_interrupt(int level) {
	printf("ack interrupt\n");
	return 0;
}

//

unsigned int board_read_byte(unsigned int address) {
	uint8_t slot = board_decode_slot(address);
	if (slot != NOCARD) {
		if (slots[slot]->read_byte != NULL) {
			return (slots[slot]->read_byte)(address & SLOT_ADDRESS_MASK);
		}
		else {
			printf("*** slot doesn't support byte read ***\n");
		}
	}
	return 0;
}

unsigned int board_read_word(unsigned int address) {
	uint8_t slot = board_decode_slot(address);
	if (slot != NOCARD) {
		if (slots[slot]->read_word != NULL) {
			return (slots[slot]->read_word)(address & SLOT_ADDRESS_MASK);
		}
		else {
			printf("*** slot doesn't support word read ***\n");
		}
	}
	return 0;
}

unsigned int board_read_long(unsigned int address) {
	uint8_t slot = board_decode_slot(address);
	if (slot != NOCARD) {
		if (slots[slot]->read_long != NULL) {
			return (slots[slot]->read_long)(address & SLOT_ADDRESS_MASK);

		}
		else {
			printf("*** slot doesn't support long read ***\n");
		}
	}
	return 0;
}

void board_write_byte(unsigned int address, unsigned int value) {
	uint8_t slot = board_decode_slot(address);
	if (slot != NOCARD) {
		if (slots[slot]->write_byte != NULL) {
			(slots[slot]->write_byte)(address & SLOT_ADDRESS_MASK, value);
		}
		else {
			printf("*** slot doesn't support byte write***\n");
		}
	}
}

void board_write_word(unsigned int address, unsigned int value) {
	uint8_t slot = board_decode_slot(address);
	if (slot != NOCARD) {
		if (slots[slot]->write_word != NULL) {
			(slots[slot]->write_word)(address & SLOT_ADDRESS_MASK, value);
		}
		else {
			printf("*** slot doesn't support word write***\n");
		}
	}
}

void board_write_long(unsigned int address, unsigned int value) {
	uint8_t slot = board_decode_slot(address);
	if (slot != NOCARD) {
		if (slots[slot]->write_long != NULL) {
			(slots[slot]->write_long)(address & SLOT_ADDRESS_MASK, value);
		}
		else {
			printf("*** slot doesn't support long write***\n");
		}
	}
}
