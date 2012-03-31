/*
 * romcard.c
 *
 *  Created on: Nov 26, 2011
 *      Author: daniel
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <stdint.h>

#include "../../musashi/m68k.h"
#include "romcard.h"
#include "../util.h"
#include "../../logging.h"
#include "../../elfloader.h"

static uint8_t* rom; /* ROM */
static uint8_t* ram; /* RAM */

static const char TAG[] = "romcard";

#define ISROMSPACE(address) (address >= ROMCARD_OFFSET_ROM && address <= MAX_ROM)
#define ISRAMSPACE(address) (address >= OFFSET_RAM && address <= MAX_RAM)

static void romcard_init() {

	if (rom == NULL) {
		rom = malloc(SIZE_ROM);
		memset(rom, 0, SIZE_ROM);
	}

	if (ram == NULL) {
		ram = malloc(SIZE_RAM);
		memset(ram, 0, SIZE_RAM);
	}

	log_println(LEVEL_INFO, TAG, "ROM section from 0x%08x to 0x%08x", ROMCARD_OFFSET_ROM, MAX_ROM);
	log_println(LEVEL_INFO, TAG, "RAM section from 0x%08x to 0x%08x", OFFSET_RAM, MAX_RAM);

}

static void romcard_dispose() {

	free(rom);
	free(ram);

}

bool romcard_loadrom(const char* path, bool elf) {

	romcard_init(); // FIXME hack

	memset(rom, 0x00, SIZE_ROM);

	if (elf) {
		return elf_load(path, rom, 0, SIZE_ROM);
	}
	else {
		FILE* fhandle;
		if ((fhandle = fopen(path, "rb")) == NULL) {
			log_println(LEVEL_WARNING, TAG, "Unable to open %s\n", path);
			return false;
		}

		log_println(LEVEL_INFO, TAG, "Loading %s into ROM", path);
		if (fread(rom, 1, SIZE_ROM + 1, fhandle) <= 0) {
			log_println(LEVEL_WARNING, TAG, "Error reading %s\n", path);
			return false;
		}

		return true;
	}
}

static bool romcard_valid_address(uint32_t address, bool write) {

	// Disallow writes to rom space, outside of ram space
	if (write) {
		if (ISRAMSPACE(address)) {
			return true;
		}
		else {
			log_println(LEVEL_INFO, TAG, "invalid write to 0x%08x, write to ROM? PC[0x%08x]", address,
					m68k_get_reg(NULL, M68K_REG_PC));
			return false;
		}
	}

	// flag reads outside of rom/ram space
	else {
		if (ISROMSPACE(address) || ISRAMSPACE(address)) {
			return true;
		}
		else {
			log_println(LEVEL_INFO, TAG, "invalid read to 0x%08x", address);
			return false;
		}
	}
}

typedef struct {
	uint8_t* base;
	uint32_t relative_address;
} bank;

static bank romcard_bank(uint32_t address) {

	bank reg;

	if (ISROMSPACE(address)) {
		reg.base = rom;
		reg.relative_address = address - ROMCARD_OFFSET_ROM;
	}

	else if (ISRAMSPACE(address)) {
		reg.base = ram;
		reg.relative_address = address - OFFSET_RAM;
	}

	return reg;
}

static uint8_t romcard_read_byte(uint32_t address) {

	if (romcard_valid_address(address, false)) {
		bank reg = romcard_bank(address);
		return READ_BYTE((reg.base), (reg.relative_address));
	}

	return 0;
}

static uint16_t romcard_read_word(uint32_t address) {

	if (romcard_valid_address(address, false)) {
		bank reg = romcard_bank(address);
		return READ_WORD(reg.base, reg.relative_address);
	}

	return 0;
}

static uint32_t romcard_read_long(uint32_t address) {

	if (romcard_valid_address(address, false)) {
		bank reg = romcard_bank(address);
		return READ_LONG(reg.base, reg.relative_address);
	}

	return 0;
}

static void romcard_write_byte(uint32_t address, uint8_t value) {

	if (romcard_valid_address(address, true)) {
		bank reg = romcard_bank(address);
		WRITE_BYTE(reg.base, reg.relative_address, value);
	}

}

static void romcard_write_word(uint32_t address, uint16_t value) {

	if (romcard_valid_address(address, true)) {
		bank reg = romcard_bank(address);
		WRITE_WORD(reg.base, reg.relative_address, value);
	}

}

static void romcard_write_long(uint32_t address, uint32_t value) {

	if (romcard_valid_address(address, true)) {
		bank reg = romcard_bank(address);
		WRITE_LONG(reg.base, reg.relative_address, value);
	}

}

const card romcard = { "ROM CARD", NULL, romcard_dispose, NULL, NULL, NULL, romcard_read_byte, romcard_read_word,
		romcard_read_long, romcard_write_byte, romcard_write_word, romcard_write_long };
