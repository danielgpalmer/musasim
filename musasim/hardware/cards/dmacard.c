/*
 * dmacard.c
 *
 *  Created on: Dec 6, 2011
 *      Author: daniel
 */

#include "dmacard.h"
#include "dmaregisters.h"
#include <stdint.h>
#include <stdbool.h>
#include "../../logging.h"
#include "../../sim.h"
#include "../board.h"

// Externally visible registers

static uint16_t config = 0;
static uint16_t datal = 0;
static uint16_t datah = 0;
static uint16_t countl = 0;
static uint16_t counth = 0;
static uint16_t sourcel = 0;
static uint16_t sourceh = 0;
static uint16_t destinationl = 0;
static uint16_t destinationh = 0;

static char* register_names[] = { "config", "data high", "data low", "count h", "count low", "source high",
		"source low", "destination h", "destination low" };
static uint16_t* dma_registers[] = { &config, &datah, &datal, &counth, &countl, &sourceh, &sourcel, &destinationh,
		&destinationl };

// Latched values
static uint32_t counter = 0;
static uint32_t data = 0;
static uint32_t source = 0;
static uint32_t destination = 0;

static const char TAG[] = "dmacard";

void dmacard_init() {

	for (int i = 0; i < 7; i++) {
		*(dma_registers[i]) = 0;
	}

}

void dmacard_irqack() {

}

static bool havebuslock = false;

void dmacard_busgrant() {

	log_println(LEVEL_DEBUG, TAG, "dmacard_busgrant");
	havebuslock = true;

}

static bool transferinprogress = false;

static uint16_t mutate(uint16_t value) {

	switch (config & DMA_REGISTER_CONFIG_MUTATOR) {
		case DMA_MUT_NOTHING:
			return value;
		case DMA_MUT_AND:
			return value & data;
		case DMA_MUT_OR:
			return value | data;
		case DMA_MUT_XOR:
			return value ^ data;

	}

	// should never get here
	return value;

}

void dmacard_tick() {

	static uint32_t* actregs[] = { &data, &source, &destination };
	static int actshifts[] = { DMA_REGISTER_CONFIG_DATAACT_SHIFT, DMA_REGISTER_CONFIG_SRCACT_SHIFT,
			DMA_REGISTER_CONFIG_DSTACT_SHIFT };

	static int state = 0; // for transfer modes to track their current phase of the unit
	static uint16_t holding = 0; // for stashing data between phases

	log_println(LEVEL_INSANE, TAG, "dmacard_tick");

	if (!havebuslock) {
		return;
	}

	for (int i = 0; i < SIM_CLOCKS_PERTICK; i++) {

		bool unitcomplete = false;

		if (transferinprogress) {
			if (counter == 0) {
				log_println(LEVEL_DEBUG, TAG, "transfer done");
				config |= DMA_REGISTER_CONFIG_DONE;
				transferinprogress = false;
				board_unlock_bus(&dmacard);
			}

			else {
				//log_println(LEVEL_DEBUG, TAG, "writing to 0x%x", destination);

				switch (config & DMA_REGISTER_CONFIG_MODE) {
					case DMA_REGISTER_CONFIG_MODE_BLOCK:
						if (state == 0) {
							if (config & DMA_REGISTER_CONFIG_SIZE) {
								holding = board_read_word(source);
							}
							else {
								holding = board_read_byte(source);
							}
							state = 1;
						}
						else {
							if (config & DMA_REGISTER_CONFIG_SIZE) {
								board_write_word(destination, mutate(holding));
							}
							else {
								board_write_byte(destination, (uint8_t)(mutate(holding) & 0xff));
							}
							state = 0;
							unitcomplete = true;
						}
						break;

					case DMA_REGISTER_CONFIG_MODE_FILL:
						if (config & DMA_REGISTER_CONFIG_SIZE) {
							board_write_word(destination, (uint16_t)(data & 0xffff));
						}
						else {
							board_write_byte(destination, (uint8_t)(data & 0xff));
						}
						unitcomplete = true;
						break;

					case DMA_REGISTER_CONFIG_MODE_MIX:
						unitcomplete = true;
						break;
					case DMA_REGISTER_CONFIG_MODE_MIXCOMPACT:
						unitcomplete = true;
						break;
				}

				if (unitcomplete) {
					// Perform actions
					for (int i = 0; i < 3; i++) {

						uint32_t* reg = actregs[i];

						int act = (config & (DMA_REGISTER_CONFIG_ACT << actshifts[i])) >> actshifts[i];

						switch (act) {
							case DMA_ACT_NOTHING:
								break;
							case DMA_ACT_INC:
								(*reg)++;
								break;
							case DMA_ACT_INCTWO:
								(*reg) += 2;
								break;
							case DMA_ACT_DEC:
								(*reg)--;
								break;
							case DMA_ACT_DECTWO:
								(*reg) -= 2;
								break;
							case DMA_ACT_ROTLEFT:
								(*reg) = ((*reg) << 1) & 0xffff;
								break;
							case DMA_ACT_ROTRIGHT:
								(*reg) = ((*reg) >> 1) & 0xffff;
								break;
							case DMA_ACT_INVERSE:
								(*reg) = ~(*reg);
								break;
						}
					}

					//log_println(LEVEL_DEBUG, TAG, "data 0x%08x, src 0x%08x, dst 0x%08x", datalatched, source, destination);

					counter--;
				}
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

	switch (config & DMA_REGISTER_CONFIG_MODE) {
		case DMA_REGISTER_CONFIG_MODE_FILL:
			log_println(LEVEL_DEBUG, TAG, "copying data 0x%08x times as %s to 0x%08x", counter,
					config & DMA_REGISTER_CONFIG_SIZE ? "words" : "bytes", destination);
			break;
		case DMA_REGISTER_CONFIG_MODE_BLOCK:
			log_println(LEVEL_DEBUG, TAG, "transferring 0x%08x %s from 0x%08x to 0x%08x", counter,
					config & DMA_REGISTER_CONFIG_SIZE ? "words" : "bytes", 0, destination);
			break;
	}

	if ((config & DMA_REGISTER_CONFIG_DSTACT_INCTWO) == DMA_REGISTER_CONFIG_DSTACT_INCTWO) {
		log_println(LEVEL_DEBUG, TAG, "dst will increment by two");
	}

}

void dmacard_write_word(uint32_t address, uint16_t value) {

// DMA transfer was started
	if (config & DMA_REGISTER_CONFIG_START) {
		if (!(config & DMA_REGISTER_CONFIG_DONE)) { // but it doesnt seem to be done yet?
			log_println(LEVEL_INFO, TAG, "Someone tried to write registers while a transfer is in the progress!");
			return;
		}
	}

	int reg = (address & ADDRESSMASK);
	log_println(LEVEL_DEBUG, TAG, "write 0x%x [%s], value 0x%x", address,
			reg == 0 ? register_names[0] : register_names[reg / 2], value);

	if (reg == DMACARD_REGISTER_CONFIG) {
		value &= 0x7fff;
	}

	*(dmacard_decodereg(address)) = value;

	if (reg == DMACARD_REGISTER_CONFIG) {
		if (value & DMA_REGISTER_CONFIG_START) {
			counter = ((counth << 16) | countl);
			source = ((sourceh << 16) || sourcel);
			destination = ((destinationh << 16) | destinationl);
			data = ((datah << 16) | datal);
			board_lock_bus(&dmacard);
			transferinprogress = true;
			value &= ~DMA_REGISTER_CONFIG_START;
			dmacard_dumpconfig();
		}
	}

}

const card dmacard = { "DMA Controller", dmacard_init, NULL, dmacard_tick, dmacard_irqack, dmacard_busgrant, NULL,
		dmacard_read_word, NULL, NULL, dmacard_write_word, NULL };

