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

static void timercard_init() {
	log_println(LEVEL_INFO, TAG, "timercard_init()");

	timer = timermodule.init();

}

static void timercard_tick(int cyclesexecuted) {
	timermodule.tick(timer);
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
		NULL, //
		NULL, //
		NULL, //
		NULL, //
		NULL, //
		NULL, //
		NULL, //
		NULL };
