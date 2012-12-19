/*
 * timercard.c
 *
 *  Created on: 2 Jun 2012
 *      Author: daniel
 */

#include <stdlib.h>
#include <inttypes.h>

#include "../../logging.h"
#include "../../utils.h"
#include "../board.h"
#include "../registerplanner.h"
#include "../modules/timer.h"

#include "card.h"
#include "timercard.h"

#include "stdio.h"

static char TAG[] = "timercard";

static uint16_t timerinterrupts = 0;
static cardaddressspace* addressspace;

static void timercard_raiseinterrupt(int index);
static void timercard_lowerinterrupt(int index);

static module_callback callback = { //
		timercard_raiseinterrupt, //
				timercard_lowerinterrupt //
		};

#define TIMERFORMAT "timer_%d"
#define BIGTIMERFORMAT "bigtimer_%d"

static cardaddressspace* timercard_setupaddressspace() {
	cardaddressspace* as = registerplanner_createaddressspace((TIMERCARD_NUMBEROFTIMERS * 2) + 1 + 1 + 1);

	unit** currentunit = as->units;
	unit* interruptregister = registerplanner_createblock(2, &timerinterrupts);
	*(currentunit++) = interruptregister;

	int index = 0;
	for (; index < TIMERCARD_NUMBEROFTIMERS; index++) {
		void* context = timermodule.init(&callback, index);

		int namelen = snprintf(NULL, 0, TIMERFORMAT, index);
		char* name = malloc(namelen + 1);
		sprintf(name, TIMERFORMAT, index);

		*(currentunit++) = registerplanner_createperipheral(&timerperipheral, name, &timermodule, context);
	}
	for (; index < (TIMERCARD_NUMBEROFTIMERS * 2); index++) {
		void* context = bigtimermodule.init(&callback, index);
		int namelen = snprintf(NULL, 0, BIGTIMERFORMAT, (index - TIMERCARD_NUMBEROFTIMERS));
		char* name = malloc(namelen + 1);
		sprintf(name, BIGTIMERFORMAT, (index - TIMERCARD_NUMBEROFTIMERS));
		*(currentunit++) = registerplanner_createperipheral(&bigtimerperipheral, name, &bigtimermodule, context);
	}

	//void* rtccontext = rtcmodule.init(&callback, index);
	*currentunit = NULL;
	registerplanner_plan(as);
	//registerplanner_print(as);
	return as;
}

static void timercard_init() {
	log_println(LEVEL_INFO, TAG, "timercard_init()");
	addressspace = timercard_setupaddressspace();
}

static void timercard_raiseinterrupt(int index) {
	//if (index < TIMERCARD_NUMBEROFTIMERS) {
	timerinterrupts |= (1 << index);
	//}
	board_raise_interrupt(&timercard);
}

static void timercard_lowerinterrupt(int index) {
	//if (index < TIMERCARD_NUMBEROFTIMERS) {
	timerinterrupts &= ~(1 << index);
	//}
	board_lower_interrupt(&timercard);
}

static void timercard_dispose() {
//	for (int t = 0; t < SIZEOFARRAY(timers); t++)
//		free(timers[t]);
}

static void timercard_tick(int cyclesexecuted) {
	registerplanner_tickmodules(addressspace, cyclesexecuted);
}

static uint16_t timercard_readword(uint32_t address) {
	return registerplanner_read_word(addressspace, address);
}

static uint32_t timercard_readlong(uint32_t address) {
	return registerplanner_read_long(addressspace, address);
}

static void timercard_writeword(uint32_t address, uint16_t value) {
	registerplanner_write_word(addressspace, address, value);
}

static void timercard_writelong(uint32_t address, uint32_t value) {
	registerplanner_write_long(addressspace, address, value);
}

static const bool timercard_isvalidaddress(uint32_t address) {
	return true;
}

static void timercard_irqack() {
	board_lower_interrupt(&timercard);
}

static int timercard_cyclesleft() {
	return registerplanner_cyclesleft(addressspace);
}

static void timercard_reset() {
	registerplanner_resetmodules(addressspace);
	timerinterrupts = 0;
}

const card timercard = { "timer card", //
		timercard_init, //
		timercard_dispose, //
		timercard_reset, //
		timercard_tick, //
		NULL, // pause
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
		timercard_cyclesleft, //
		timercard_setupaddressspace //
		};

