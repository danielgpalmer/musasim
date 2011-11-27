/*
 * romcard.c
 *
 *  Created on: Nov 26, 2011
 *      Author: daniel
 */

#include "romcard.h"
#include "stdio.h"
#include "string.h"
#include "stdbool.h"
#include "stdint.h"

unsigned char g_rom[SIZE_ROM]; /* ROM */
unsigned char g_ram[SIZE_RAM]; /* RAM */

bool romcard_loadrom(char* path) {

	memset(g_rom, 0x00, SIZE_ROM);
	FILE* fhandle;
	if ((fhandle = fopen(path, "rb")) == NULL) {
		printf("Unable to open %s\n", path);
		return false;
	}

	printf("Loading %s into ROM\n", path);
	if (fread(g_rom, 1, SIZE_ROM + 1, fhandle) <= 0) {
		printf("Error reading %s\n", path);
		return false;
	}

	return true;
}

uint8_t romcard_read_byte(uint32_t address) {
	return 0;
}

uint16_t romcard_read_word(uint32_t address) {
	return 0;
}

uint32_t romcard_read_long(uint32_t address) {
	return 0;
}

void romcard_write_byte(uint32_t address, uint8_t value) {

}

void romcard_write_word(uint32_t address, uint16_t value) {

}

void romcard_write_long(uint32_t address, uint32_t value) {

}

card romcard = { "ROM CARD", romcard_read_byte, romcard_read_word, romcard_read_long, romcard_write_byte,
		romcard_write_word, romcard_write_long };
