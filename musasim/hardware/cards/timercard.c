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

static char TAG[] = "timercard";

void* timer;

static void timercard_raiseinterrupt() {

}

static void timercard_lowerinterrupt() {

}

static module_callback callback = { //
		timercard_raiseinterrupt, //
				timercard_lowerinterrupt //
		};

static void timercard_init() {
	log_println(LEVEL_INFO, TAG, "timercard_init()");

	timer = timermodule.init(&callback);

}

static void timercard_dispose() {
	free(timer);
}

static void timercard_tick(int cyclesexecuted) {
	timermodule.tick(timer);
}

static uint16_t timercard_readword(uint32_t address) {
	return timermodule.read_word(timer, (uint16_t) (address & 0xf));
}

static void timercard_writeword(uint32_t address, uint16_t value) {
	timermodule.write_word(timer, (uint16_t) (address & 0xf), value);
}

const card timercard = { "timer card", //
		timercard_init, //
		timercard_dispose, //
		NULL, //
		timercard_tick, //
		NULL, //
		NULL, //
		NULL, //
		NULL, //
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

