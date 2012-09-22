/*
 * timercard.c
 *
 *  Created on: 2 Jun 2012
 *      Author: daniel
 */

#include <stdlib.h>

#include "../../logging.h"
#include "card.h"
#include "timercard.h"
#include "../modules/timer.h"
#include "../board.h"
#include "../../utils.h"

static char TAG[] = "timercard";

void* timers[TIMERCARD_NUMBEROFTIMERS];

static void timercard_raiseinterrupt() {
	board_raise_interrupt(&timercard);
}

static void timercard_lowerinterrupt() {
	board_lower_interrupt(&timercard);
}

static module_callback callback = { //
		timercard_raiseinterrupt, //
				timercard_lowerinterrupt //
		};

static void timercard_init() {
	log_println(LEVEL_INFO, TAG, "timercard_init()");

	for (int t = 0; t < SIZEOFARRAY(timers); t++)
		timers[t] = timermodule.init(&callback);

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
	return timermodule.read_word(timers[0], (uint16_t) (address & 0xf));
}

static void timercard_writeword(uint32_t address, uint16_t value) {
	timermodule.write_word(timers[0], (uint16_t) (address & 0xf), value);
}

static bool timercard_isvalidaddress(uint32_t address) {
	return true;
}

static void timercard_irqack() {
	board_lower_interrupt(&timercard);
}

const card timercard = { "timer card", //
		timercard_init, //
		timercard_dispose, //
		NULL, //
		timercard_tick, //
		timercard_irqack, //
		NULL, //
		NULL, //
		timercard_isvalidaddress, //
		NULL, //
		NULL, //
		timercard_readword, //
		NULL, //
		NULL, //
		timercard_writeword, //
		NULL, //
		NULL, //
		NULL, //
		NULL };

