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

bool isromspace(unsigned int address);
bool isramspace(unsigned int address);
bool ismagicspace(unsigned int address);
bool ishardwarespace(unsigned int address);

bool isromspace(unsigned int address) {

	if (address >= OFFSET_ROM && address <= (MAX_ROM)) {
		return true;
	}

	return false;
}

bool isramspace(unsigned int address) {

	if (address >= OFFSET_RAM && address <= (MAX_RAM)) {
		return true;
	}

	return false;
}

bool ismagicspace(unsigned int address) {

	if (address >= OFFSET_MAGIC && address <= (MAX_MAGIC)) {
		return true;
	}

	return false;
}

bool ishardwarespace(unsigned int address) {

	if (address >= OFFSET_VIDEO && address <= MAX_VIDEO) {
		return true;
	}

	return false;
}

void badread(unsigned int address) {
	fprintf(stderr, "Attempted to read byte from address %08x\n", address);
}

void badwriterom(unsigned int address) {
	fprintf(stderr, "Attempted to write to ROM address %08x\n", address);
}

void badwrite(unsigned int address) {
	fprintf(stderr, "Attempted to write to address %08x\n", address);
}

card* slots[NUM_SLOTS];

void decode_slot(uint32_t address) {

	uint8_t slot = (address & 0xE00000) >> 21;
	printf("Slot %d\n", slot);

	if (slots[slot] == NULL) {
		printf("*** NO CARD IN SLOT ***\n");
	}
	else {
		printf("*** %s ***\n", slots[slot]->boardinfo);
	}


}

void board_add_device(uint8_t slot, card *card) {
	slots[slot] = card;
}

/* Read data from RAM, ROM, or a device */
unsigned int cpu_read_byte(unsigned int address) {

	decode_slot(address);

	unsigned int value = 0;

	if (isromspace(address)) /* Program */
	{
		value = READ_BYTE(g_rom, address - OFFSET_ROM);
	}

	if (isramspace(address)) {
		value = READ_BYTE(g_ram, address - OFFSET_RAM);
	}

	if (ishardwarespace(address)) {
		value = (uint32_t) video_read_byte(address - OFFSET_VIDEO);
	}

	badread(address);

	printf("Read 0x%x from 0x%x\n", value, address);

	return value;

}

unsigned int cpu_read_word(unsigned int address) {

	decode_slot(address);

	if (isromspace(address)) { /* Program */
		return READ_WORD(g_rom, address - OFFSET_ROM);
	}

	if (isramspace(address)) {
		return READ_WORD(g_ram, address - OFFSET_RAM);
	}

	badread(address);
	return 0;

}

unsigned int cpu_read_long(unsigned int address) {

	decode_slot(address);

	if (isromspace(address)) { /* Program */
		return READ_LONG(g_rom, address - OFFSET_ROM);
	}

	if (isramspace(address)) {
		return READ_LONG(g_ram, address - OFFSET_RAM);
	}

	badread(address);

	return 0;

}

/* Write data to RAM or a device */
void cpu_write_byte(unsigned int address, unsigned int value) {

	decode_slot(address);

	if (isromspace(address)) { /* Program */
		badwriterom(address);
		return;
	}

	else if (isramspace(address)) {
		printf("writing byte 0x%x to 0x%x\n", value, address);
		WRITE_BYTE(g_ram, address - OFFSET_RAM, value);
	}

	else if (ismagicspace(address)) {
		printf("%c", (char) value);
	}

	else if (ishardwarespace(address)) {
		video_write_byte(address - OFFSET_VIDEO, (uint8_t) value);
	}

	else {
		badwrite(address);
	}
}

void cpu_write_word(unsigned int address, unsigned int value) {

	decode_slot(address);

	if (isromspace(address)) { /* Program */
		badwriterom(address);
		return;
	}

	else if (isramspace(address)) {
		WRITE_WORD(g_ram, address - OFFSET_RAM, value);
	}

	else if (ishardwarespace(address)) {
		video_write_word(address - OFFSET_VIDEO, (uint16_t) value);
	}

}

void cpu_write_long(unsigned int address, unsigned int value) {

	decode_slot(address);

	if (isromspace(address)) { /* Program */
		badwriterom(address);
		return;
	}

	if (isramspace(address)) {
		WRITE_LONG(g_ram, address - OFFSET_RAM, value);
	}
}
