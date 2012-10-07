/*
 * timercard.c
 *
 *  Created on: 2 Jun 2012
 *      Author: daniel
 */

#include <stdlib.h>

#include "../../logging.h"
#include "../../utils.h"
#include "../board.h"

#include "card.h"
#include "timercard.h"

static char TAG[] = "timercard";

#define TIMERFROMADDRESS(a) ((a & 0x30) >> 4)

uint16_t timerinterrupts = 0;
static void* timers[TIMERCARD_NUMBEROFTIMERS];
static void* bigtimers[TIMERCARD_NUMBEROFTIMERS];
static void* rtc;

static void timercard_raiseinterrupt(int index) {

	if (index < TIMERCARD_NUMBEROFTIMERS) {
		timerinterrupts |= (1 << index);
	}
	board_raise_interrupt(&timercard);

}

static void timercard_lowerinterrupt(int index) {
	if (index < TIMERCARD_NUMBEROFTIMERS) {
		timerinterrupts &= ~(1 << index);
	}
	board_lower_interrupt(&timercard);
}

static module_callback callback = { //
		timercard_raiseinterrupt, //
				timercard_lowerinterrupt //
		};

static void timercard_init() {
	log_println(LEVEL_INFO, TAG, "timercard_init()");

	for (int t = 0; t < SIZEOFARRAY(timers); t++)
		timers[t] = timermodule.init(&callback, t);
	for (int t = 0; t < SIZEOFARRAY(bigtimers); t++)
		timers[t] = bigtimermodule.init(&callback, t);

	rtc = rtcmodule.init(&callback, SIZEOFARRAY(timers) + 1);

}

static void timercard_dispose() {
	for (int t = 0; t < SIZEOFARRAY(timers); t++)
		free(timers[t]);
}

static void timercard_tick(int cyclesexecuted) {
	for (int t = 0; t < SIZEOFARRAY(timers); t++)
		timermodule.tick(timers[t], cyclesexecuted);
}

static uint16_t timercard_readword(uint32_t address) {
	if (address == 0)
		return timerinterrupts;

	return timermodule.read_word(timers[TIMERFROMADDRESS(address)], (uint16_t) (address & 0xf));
}

static uint32_t timercard_readlong(uint32_t address) {
	return 0;
}

static void timercard_writeword(uint32_t address, uint16_t value) {
	if (address == 0)
		return;

	timermodule.write_word(timers[TIMERFROMADDRESS(address)], (uint16_t) (address & 0xf), value);
}

static void timercard_writelong(uint32_t address, uint32_t value) {

}

static bool timercard_isvalidaddress(uint32_t address) {
	return true;
}

static void timercard_irqack() {
	board_lower_interrupt(&timercard);
}

static int timercard_cyclesleft() {

	int cycles = -1;
	for (int i = 0; i < SIZEOFARRAY(timers); i++) {
		int timercycles = timermodule.cyclesleft(timers[i]);
		if (timercycles > cycles)
			cycles = timercycles;
	}

	return cycles;
}

static void timercard_reset() {
	timerinterrupts = 0;
}

const card timercard = { "timer card", //
		timercard_init, //
		timercard_dispose, //
		timercard_reset, //
		timercard_tick, //
		timercard_irqack, //
		NULL, //
		NULL, //
		timercard_isvalidaddress, //
		NULL, //
		NULL, //
		timercard_readword, //
		timercard_readlong, //
		NULL, //
		timercard_writeword, //
		timercard_writelong, //
		NULL, //
		NULL, //
		timercard_cyclesleft //
		};

