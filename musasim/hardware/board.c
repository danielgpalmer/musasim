#include <stdint.h>
#include <stdbool.h>
#include "board.h"
#include "stdio.h"

/*
 *
 *
 *
 */

card* slots[NUM_SLOTS];

#define NOCARD 0xFF

uint8_t decode_slot(uint32_t address) {

	uint8_t slot = (address & 0xE00000) >> 21;
	printf("Slot %d\n", slot);

	if (slots[slot] == NULL) {
		printf("*** NO CARD IN SLOT ***\n");
		return NOCARD;
	}
	else {
		printf("*** %s ***\n", slots[slot]->boardinfo);
	}

	return slot;

}

void board_add_device(uint8_t slot, card *card) {
	slots[slot] = card;
	if (card->init != NULL) {
		(card->init)();
	}
}

unsigned int cpu_read_byte(unsigned int address) {
	uint8_t slot = decode_slot(address);
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

unsigned int cpu_read_word(unsigned int address) {
	uint8_t slot = decode_slot(address);
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

unsigned int cpu_read_long(unsigned int address) {
	uint8_t slot = decode_slot(address);
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

void cpu_write_byte(unsigned int address, unsigned int value) {
	uint8_t slot = decode_slot(address);
	if (slot != NOCARD) {
		if (slots[slot]->write_byte != NULL) {
			(slots[slot]->write_byte)(address & SLOT_ADDRESS_MASK, value);
		}
		else {
			printf("*** slot doesn't support byte write***\n");
		}
	}
}

void cpu_write_word(unsigned int address, unsigned int value) {
	uint8_t slot = decode_slot(address);
	if (slot != NOCARD) {
		if (slots[slot]->write_word != NULL) {
			(slots[slot]->write_word)(address & SLOT_ADDRESS_MASK, value);
		}
		else {
			printf("*** slot doesn't support word write***\n");
		}
	}
}

void cpu_write_long(unsigned int address, unsigned int value) {
	uint8_t slot = decode_slot(address);
	if (slot != NOCARD) {
		if (slots[slot]->write_long != NULL) {
			(slots[slot]->write_long)(address & SLOT_ADDRESS_MASK, value);
		}
		else {
			printf("*** slot doesn't support long write***\n");
		}
	}
}
