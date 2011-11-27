#include <stdint.h>
#include <stdbool.h>
#include "board.h"
#include "cards/videocard.h"
#include "stdio.h"

/*
 *
 *
 *
 */

card* slots[NUM_SLOTS];

uint8_t decode_slot(uint32_t address) {

	uint8_t slot = (address & 0xE00000) >> 21;
	printf("Slot %d\n", slot);

	if (slots[slot] == NULL) {
		printf("*** NO CARD IN SLOT ***\n");
	}
	else {
		printf("*** %s ***\n", slots[slot]->boardinfo);
	}

	return slot;

}

void board_add_device(uint8_t slot, card *card) {
	slots[slot] = card;
}

unsigned int cpu_read_byte(unsigned int address) {
	return (slots[decode_slot(address)]->read_byte)(address & SLOT_ADDRESS_MASK);
}

unsigned int cpu_read_word(unsigned int address) {
	return (slots[decode_slot(address)]->read_word)(address & SLOT_ADDRESS_MASK);
}

unsigned int cpu_read_long(unsigned int address) {
	return (slots[decode_slot(address)]->read_long)(address & SLOT_ADDRESS_MASK);
}

void cpu_write_byte(unsigned int address, unsigned int value) {
	(slots[decode_slot(address)]->write_byte)(address & SLOT_ADDRESS_MASK, value);
}

void cpu_write_word(unsigned int address, unsigned int value) {
	(slots[decode_slot(address)]->write_word)(address & SLOT_ADDRESS_MASK, value);
}

void cpu_write_long(unsigned int address, unsigned int value) {
	(slots[decode_slot(address)]->write_word)(address & SLOT_ADDRESS_MASK, value);
}
