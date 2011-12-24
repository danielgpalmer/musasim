/*
 * dmacard.c
 *
 *  Created on: Dec 6, 2011
 *      Author: daniel
 */

#ifndef DMACARD_C_
#define DMACARD_C_

#include "dmacard.h"
#include "dmaregisters.h"
#include <stdint.h>
#include <stdbool.h>
#include "../../logging.h"
#include "../../sim.h"
#include "../board.h"

static uint16_t config = 0;
static uint16_t count = 0;
static uint16_t sourcel = 0;
static uint16_t sourceh = 0;
static uint16_t destinationl = 0;
static uint16_t destinationh = 0;

static uint16_t* dma_registers[] = { &config, &count, &sourceh, &sourcel, &destinationh, &destinationl };

static const char TAG[] = "dmacard";

void dmacard_init() {

	for (int i = 0; i < 6; i++) {
		*(dma_registers[i]) = 0;
	}

}

void dmacard_dispose() {

}

void dmacard_irqack() {

}

static bool havebuslock = false;

void dmacard_busgrant() {

	log_println(LEVEL_DEBUG, TAG, "dmacard_busgrant");
	havebuslock = true;

}

static bool transferinprogress = false;

void dmacard_tick() {

	log_println(LEVEL_INSANE, TAG, "dmacard_tick");

	if (!havebuslock) {
		return;
	}

	for (int i = 0; i < SIM_CLOCKS_PERTICK; i++) {
		if (transferinprogress) {
			if (count == 0) {
				log_println(LEVEL_DEBUG, TAG, "transfer done");
				config |= DMA_REGISTER_CONFIG_DONE;
				transferinprogress = false;
				board_unlock_bus(&dmacard);
			}

			else {
				uint32_t address = (destinationh << 16) | destinationl;
				//log_println(LEVEL_DEBUG, TAG, "writing to 0x%x", address);

				if (config & DMA_REGISTER_CONFIG_SIZE) {
					board_write_word(address, 0xFFFF);
					address += 2;
				}
				else {
					board_write_byte(address, 0xFF);
					address += 1;
				}

				count--;
				destinationh = (address >> 16) & 0xFFFF;
				destinationl = address & 0xFFFFF;
			}

		}
	}
}

#define ADDRESSMASK 0x0F

static uint16_t* dmacard_decodereg(uint32_t address) {
	int reg = (address & ADDRESSMASK);
	if (reg != 0) {
		reg /= 2;
	}
	return dma_registers[reg];
}

uint16_t dmacard_read_word(uint32_t address) {
	return *(dmacard_decodereg(address));
}

void dmacard_dumpconfig() {

}

void dmacard_write_word(uint32_t address, uint16_t value) {
	int reg = (address & ADDRESSMASK);
	log_println(LEVEL_DEBUG, TAG, "write %d, value %d", address, value);

	switch (reg) {
		case DMACARD_REGISTER_CONFIG:
			value &= 0x7fff;
			if (value & DMA_REGISTER_CONFIG_START) {
				board_lock_bus(&dmacard);
				transferinprogress = true;
				value &= ~DMA_REGISTER_CONFIG_START;
			}
			break;
		case DMACARD_REGISTER_COUNT:
			log_println(LEVEL_DEBUG, TAG, "Transfer count set to %d", value);
			break;
	}

	*(dmacard_decodereg(address)) = value;

}

card dmacard = { "DMA Controller", dmacard_init, dmacard_dispose, dmacard_tick, dmacard_irqack, dmacard_busgrant, NULL,
		dmacard_read_word, NULL, NULL, dmacard_write_word, NULL };

#endif /* DMACARD_C_ */
