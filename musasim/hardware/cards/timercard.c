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
static cardaddressspace addressspace;

static void timercard_raiseinterrupt(int index);
static void timercard_lowerinterrupt(int index);

static module_callback callback = { //
		timercard_raiseinterrupt, //
				timercard_lowerinterrupt //
		};

static void timercard_init() {
	log_println(LEVEL_INFO, TAG, "timercard_init()");

	// allocate enough unit pointers for the interrupt status registe ,
	// a set of timers and a set of bigtimers an rtc module, and a NULL terminator
	unit** units = malloc(sizeof(unit*) * ((TIMERCARD_NUMBEROFTIMERS * 2) + 1 + 1 + 1));
	unit** currentunit = units;

	unit* interruptregister = registerplanner_createblock(2, &timerinterrupts);
	*(currentunit++) = interruptregister;

	int index = 0;
	for (; index < TIMERCARD_NUMBEROFTIMERS; index++) {
		void* context = timermodule.init(&callback, index);
		*(currentunit++) = registerplanner_createperipheral(&timerperipheral, &timermodule, context);
	}
	for (; index < (TIMERCARD_NUMBEROFTIMERS * 2); index++) {
		void* context = bigtimermodule.init(&callback, index);
		*(currentunit++) = registerplanner_createperipheral(&bigtimerperipheral, &bigtimermodule, context);
	}

	//void* rtccontext = rtcmodule.init(&callback, index);
	*currentunit = NULL;
	addressspace.units = units;
	registerplanner_plan(&addressspace);
	registerplanner_print(&addressspace);
}

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

static void timercard_dispose() {
//	for (int t = 0; t < SIZEOFARRAY(timers); t++)
//		free(timers[t]);
}

static void timercard_tick(int cyclesexecuted) {
	registerplanner_tickmodules(&addressspace, cyclesexecuted);
}

static uint16_t timercard_readword(uint32_t address) {
	return registerplanner_read_word(&addressspace, address);
}

static uint32_t timercard_readlong(uint32_t address) {
	return registerplanner_read_long(&addressspace, address);
}

static void timercard_writeword(uint32_t address, uint16_t value) {
	registerplanner_write_word(&addressspace, address, value);
}

static void timercard_writelong(uint32_t address, uint32_t value) {
	registerplanner_write_long(&addressspace, address, value);
}

static bool timercard_isvalidaddress(uint32_t address) {
	return true;
}

static void timercard_irqack() {
	board_lower_interrupt(&timercard);
}

static int timercard_cyclesleft() {

//int cycles = -1;
//for (int i = 0; i < SIZEOFARRAY(timers); i++) {
//	int timercycles = timermodule.cyclesleft(timers[i]);
//	if (timercycles > cycles)
//		cycles = timercycles;
//}

//return cycles;
	return 0;
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

