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

#ifdef GDBSERVER
#include "../../gdbserver.h"
#endif

#include "romcard.h"
#include "../util.h"
#include "../../musashi/m68k.h"
#include "../../logging.h"
#include "../../elfloader.h"

static uint8_t* bootloaderrom = NULL; //BOOTLOADER ROM
static uint8_t* rom = NULL; //ROM
static uint8_t* ram = NULL; // RAM
static bool registersenabled = false;
static bool disableromonreset = false;
static bool romdisabled = false;

static const char TAG[] = "romcard";

#define ISROMSPACE(address) (address >= ROMCARD_OFFSET_ROM && address <= MAX_ROM)
#define ISRAMSPACE(address) (address >= OFFSET_RAM && address <= MAX_RAM)

static void romcard_init() {
	if (bootloaderrom == NULL ) {
		bootloaderrom = malloc(SIZE_ROM);
	}

	if (rom == NULL ) {
		rom = malloc(SIZE_ROM);
		if (rom == NULL )
			log_println(LEVEL_INFO, TAG, "Failed to malloc ram!");
		memset(rom, 0, SIZE_ROM);
	}

	if (ram == NULL ) {
		ram = malloc(SIZE_RAM);
		if (ram == NULL )
			log_println(LEVEL_INFO, TAG, "Failed to malloc ram!");
		memset(ram, 0x00, SIZE_RAM);
	}

	log_println(LEVEL_INFO, TAG, "ROM section from 0x%08x to 0x%08x", ROMCARD_OFFSET_ROM, MAX_ROM);
	log_println(LEVEL_INFO, TAG, "RAM section from 0x%08x to 0x%08x", OFFSET_RAM, MAX_RAM);
}

static void romcard_dispose() {
	free(bootloaderrom);
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
		if ((fhandle = fopen(path, "rb")) == NULL ) {
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

static inline bool romcard_checkcommand(uint32_t address, uint16_t data) __attribute__((always_inline));
static inline bool romcard_checkcommand(uint32_t address, uint16_t data) {
	static bool disablecommandone = false;

	if (!registersenabled)
		return false;

	if (address == ROMDISABLE_ADDRESS_0 && data == ROMDISABLE_DATA_0) {
		log_println(LEVEL_WARNING, TAG, "ROM disable armed");
		disablecommandone = true;
		return true;
	}

	else if (address == ROMDISABLE_ADDRESS_1 && data == ROMDISABLE_DATA_1 && disablecommandone) {
		log_println(LEVEL_WARNING, TAG, "ROM disable fired, ROM will be disabled on reset");
		disableromonreset = true;
		return true;
	}

	else {
		disablecommandone = false;
		return false;
	}
}

static inline void romcard_invalidwrite(uint32_t address) {
	log_println(LEVEL_INFO, TAG, "invalid write to 0x%08x, write to ROM? PC[0x%08x]", address,
			m68k_get_reg(NULL, M68K_REG_PC));
#ifdef GDBSERVER
	gdb_break("write to rom");
#endif

}

typedef struct {
	uint8_t* base;
	uint32_t relative_address;
	bool writeable;
} translatedaddress;

static inline translatedaddress* romcard_translateaddress(uint32_t address) __attribute__((always_inline));
static inline translatedaddress* romcard_translateaddress(uint32_t address) {
	static translatedaddress reg;
	reg.writeable = true;
	if (ISROMSPACE(address)) {
		if (romdisabled) {
			reg.base = ram;
			reg.relative_address = address - ROMCARD_OFFSET_ROM;
		}
		else {
			reg.base = rom;
			reg.relative_address = address - ROMCARD_OFFSET_ROM;
			reg.writeable = false;

		}
	}
	else if (ISRAMSPACE(address)) {
		reg.base = ram;
		reg.relative_address = address - OFFSET_RAM;
	}
	return &reg;
}

static uint8_t romcard_read_byte(uint32_t address) {
	translatedaddress* translated = romcard_translateaddress(address);
	return READ_BYTE((translated->base), (translated->relative_address));
}

static uint16_t romcard_read_word(uint32_t address) {
	translatedaddress* translated = romcard_translateaddress(address);
	return READ_WORD(translated->base, translated->relative_address);
}

static uint32_t romcard_read_long(uint32_t address) {
	translatedaddress* translated = romcard_translateaddress(address);
	return READ_LONG(translated->base, translated->relative_address);
}

static void romcard_write_byte(uint32_t address, uint8_t value) {
	if (!romcard_checkcommand(address, value)) {
		translatedaddress* translated = romcard_translateaddress(address);
		if (translated->writeable) {
			WRITE_BYTE(translated->base, translated->relative_address, value);
		}
		else {
			romcard_invalidwrite(address);
		}
	}
}

static void romcard_write_word(uint32_t address, uint16_t value) {
	if (!romcard_checkcommand(address, value)) {
		translatedaddress* translated = romcard_translateaddress(address);
		if (translated->writeable) {
			WRITE_WORD(translated->base, translated->relative_address, value);
		}
		else {
			romcard_invalidwrite(address);
		}
	}
}

static void romcard_write_long(uint32_t address, uint32_t value) {
	translatedaddress* translated = romcard_translateaddress(address);
	if (translated->writeable) {
		WRITE_LONG(translated->base, translated->relative_address, value);
	}
	else {
		romcard_invalidwrite(address);
	}
}

static const bool romcard_validaddress(uint32_t address) {
	if (ISROMSPACE(address) || ISRAMSPACE(address)) {
		return true;
	}
	return false;
}

static void romcard_reset() {
	if (disableromonreset) {
		log_println(LEVEL_WARNING, TAG, "ROM is disabled");
		disableromonreset = false;
		romdisabled = true;
	}
	else {
		romdisabled = false;
	}
}

static void romcard_setfc(unsigned int fc) {
	if (fc == 0x5 || fc == 0x6)
		registersenabled = true;
	else
		registersenabled = false;
}

const static uint8_t romcard_memorytype(uint32_t address) {
	if (ISRAMSPACE(address))
		return CARDMEMORYTYPE_WRITABLE;
	else if (ISROMSPACE(address))
		return CARDMEMORYTYPE_EXECUTABLE | CARDMEMORYTYPE_READABLE;
	else
		return 0;
}

const card romcard = { "ROM CARD", //
		romcard_init, //
		romcard_dispose, //
		romcard_reset, // reset
		NULL, //
		NULL, // pause
		NULL, //
		NULL, //
		romcard_setfc, //
		romcard_validaddress, //
		romcard_memorytype, //
		romcard_read_byte, //
		romcard_read_word, //
		romcard_read_long, //
		romcard_write_byte, //
		romcard_write_word, //
		romcard_write_long, //
		NULL, //
		NULL, //
		NULL, //
		NULL };
