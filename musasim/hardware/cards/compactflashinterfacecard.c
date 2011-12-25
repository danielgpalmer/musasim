/*
 * compactflashinterfacecard.c
 *
 *  Created on: Dec 3, 2011
 *      Author: daniel
 */

#include "compactflashinterfacecard.h"

#include <stdint.h>
#include "../../logging.h"

const static char TAG[] = "cfint";

static uint16_t dataport;
static uint16_t statusport;

static uint16_t* registers[] = { &dataport, NULL, NULL, NULL, NULL, NULL, &statusport };

void cfintf_load(char* filename) {
	log_println(LEVEL_INFO, TAG, "cfload");
}

#define ADDRESSMASK 0b111

#define REG(address) (address & ADDRESSMASK)

uint8_t cfintf_read_byte(uint32_t address) {
	return *(registers[REG(address)]);
}

uint16_t cfintf_read_word(uint32_t address) {
	int reg = REG(address);
	if (reg != 0) {
		return 0;
	}
	return *(registers[REG(address)]);
}

void cfintf_write_byte(uint32_t address, uint8_t value) {
	*(registers[REG(address)]) = value;
}

void cfintf_write_word(uint32_t address, uint16_t value) {
	int reg = REG(address);
	if (reg != 0) {
		return;
	}

}

const card compactflashinterfacecard = { "CF INTERFACE", NULL, NULL, NULL, NULL, NULL, cfintf_read_byte,
		cfintf_read_word, NULL, cfintf_write_byte, cfintf_write_word, NULL };
