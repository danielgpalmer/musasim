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
#include "stdint.h"
#include "../../logging.h"

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

void dmacard_busgrant() {

}

void dmacard_tick() {

	log_println(LEVEL_DEBUG, TAG, "dmacard_tick");

	config |= DMA_REGISTER_CONFIG_DONE;

}

#define ADDRESSMASK 0x07

static int dmacard_decodereg(uint32_t address) {
	return (address & ADDRESSMASK);
}

uint16_t dmacard_read_word(uint32_t address) {
	return *(dma_registers[dmacard_decodereg(address)]);
}

void dmacard_write_word(uint32_t address, uint16_t value) {

	*(dma_registers[dmacard_decodereg(address)]) = value;

}

card dmacard = { "DMA Controller", dmacard_init, dmacard_dispose, dmacard_tick, dmacard_irqack, dmacard_busgrant, NULL,
		dmacard_read_word, NULL, NULL, dmacard_write_word, NULL };

#endif /* DMACARD_C_ */
