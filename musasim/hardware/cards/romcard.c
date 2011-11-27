/*
 * romcard.c
 *
 *  Created on: Nov 26, 2011
 *      Author: daniel
 */

#include "stdio.h"
#include "string.h"
#include "stdbool.h"
#include "stdint.h"

#include "romcard.h"
#include "../util.h"

uint8_t rom[SIZE_ROM]; /* ROM */
uint8_t ram[SIZE_RAM]; /* RAM */

bool romcard_loadrom(char* path) {

	memset(rom, 0x00, SIZE_ROM);
	FILE* fhandle;
	if ((fhandle = fopen(path, "rb")) == NULL) {
		printf("Unable to open %s\n", path);
		return false;
	}

	printf("Loading %s into ROM\n", path);
	if (fread(rom, 1, SIZE_ROM + 1, fhandle) <= 0) {
		printf("Error reading %s\n", path);
		return false;
	}

	return true;
}

bool romcard_valid_address(uint32_t address, bool write) {
	if (address < MAX_RAM) {
		return true;
	}

	return false;
}

typedef struct {
	uint8_t* base;
	uint32_t relative_address;
} bank;

bank romcard_bank(uint32_t address) {

	bank reg;

	if (address >= OFFSET_ROM && address <= MAX_ROM) {
		reg.base = rom;
		reg.relative_address = address - OFFSET_ROM;
	}

	else if (address >= OFFSET_RAM && address <= MAX_RAM) {
		reg.base = ram;
		reg.relative_address = address - OFFSET_RAM;
	}

	return reg;
}

uint8_t romcard_read_byte(uint32_t address) {

	if (romcard_valid_address(address, false)) {
		bank reg = romcard_bank(address);
		return READ_BYTE((reg.base), (reg.relative_address));
	}

	return 0;
}

uint16_t romcard_read_word(uint32_t address) {

	if (romcard_valid_address(address, false)) {
		bank reg = romcard_bank(address);
		return READ_WORD(reg.base, reg.relative_address);
	}

	return 0;
}

uint32_t romcard_read_long(uint32_t address) {

	if (romcard_valid_address(address, true)) {
		bank reg = romcard_bank(address);
		return READ_LONG(reg.base, reg.relative_address);
	}

	return 0;
}

void romcard_write_byte(uint32_t address, uint8_t value) {

	if (romcard_valid_address(address, true)) {
		bank reg = romcard_bank(address);
		WRITE_BYTE(reg.base, reg.relative_address, value);
	}

}

void romcard_write_word(uint32_t address, uint16_t value) {

	if (romcard_valid_address(address, true)) {
		bank reg = romcard_bank(address);
		WRITE_WORD(reg.base, reg.relative_address, value);
	}

}

void romcard_write_long(uint32_t address, uint32_t value) {

	if (romcard_valid_address(address, true)) {
		bank reg = romcard_bank(address);
		WRITE_LONG(reg.base, reg.relative_address, value);
	}

}

card romcard = { "ROM CARD", romcard_read_byte, romcard_read_word, romcard_read_long, romcard_write_byte,
		romcard_write_word, romcard_write_long };
