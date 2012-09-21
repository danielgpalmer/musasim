/*
 * timercard.c
 *
 *  Created on: 2 Jun 2012
 *      Author: daniel
 */

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

static void timercard_tick(int cyclesexecuted) {
	timermodule.tick(timer);
}

static uint16_t timercard_readword(uint32_t address) {
	return 0;
}

static void timercard_writeword(uint32_t address, uint16_t value) {

}

const card timercard = { "timer card", //
		timercard_init, //
		NULL, //
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

