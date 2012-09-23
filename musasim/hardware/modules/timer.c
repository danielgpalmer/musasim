/*
 * timer.c
 *
 *  Created on: Jan 19, 2012
 *      Author: daniel
 */

#include <stdint.h>
#include <stddef.h>
#include <stdlib.h>
#include <inttypes.h>
#include <timers_registermasks.h>
#include "timer.h"
#include "module.h"
#include "../../logging.h"

#define TAG "timermodule"

typedef struct {
	module_callback* cb;
	uint16_t flags;
	uint16_t config;
	uint16_t prescaler;
	uint16_t prescalercounter;
	uint16_t counter;
	uint16_t matcha;
	uint16_t matchb;
} context_t;

/* flags
 * f|e|d|c|b|a|9|8|7|6|5|4|3|2|1|0
 *  | | | | | | | | | | | | | |I|i
 */

#define FLAGGED_INTERRUPTMATCHA(c) (c->flags & TIMERS_REGISTER_FLAGS_MATCHAINT)
#define FLAGGED_INTERRUPTMATCHB(c) (c->flags & TIMERS_REGISTER_FLAGS_MATCHBINT)

#define SET_FLAG_INTERRUPTMATCHA(c) (c->flags |= TIMERS_REGISTER_FLAGS_MATCHAINT)
#define SET_FLAG_INTERRUPTMATCHB(c) (c->flags |= TIMERS_REGISTER_FLAGS_MATCHBINT)
#define CLEAR_FLAG_INTERRUPTMATCHA(c) (c->flags &= ~TIMERS_REGISTER_FLAGS_MATCHAINT)
#define CLEAR_FLAG_INTERRUPTMATCHB(c) (c->flags &= ~TIMERS_REGISTER_FLAGS_MATCHBINT)

/* config
 *
 * f|e|d|c|b|a|9|8|7|6|5|4|3|2|1|0
 *  | | | | | | | | | | |R|r|I|i|e
 *
 *  R - reset counter on match a
 *  r - reset counter on match b
 *  I - interrupt on match b
 *  i - interrupt on match a
 *  e - enable
 */

#define ENABLED(c) (c->config & TIMERS_REGISTER_CONFIG_ENABLE)

#define MATCHA_RESET(c) (c->config & TIMERS_REGISTER_CONFIG_RESETMATCHA)
#define MATCHB_RESET(c) (c->config & TIMERS_REGISTER_CONFIG_RESETMATCHB)

#define MATCHA_INTERRUPT_ENABLED(c) (c->config & TIMERS_REGISTER_CONFIG_ENMATCHAINT)
#define MATCHB_INTERRUPT_ENABLED(c) (c->config & TIMERS_REGISTER_CONFIG_ENMATCHBINT)

static void* timer_init(module_callback* callback) {
	context_t * c = calloc(sizeof(context_t), 1);
	c->cb = callback;
	return c;
}

static void timer_tick(void* context, int cycles) {

	context_t* c = (context_t*) context;

	if (!ENABLED(c))
		return;

	for (; cycles > 0; cycles--) {
		if (c->prescaler == c->prescalercounter) {
			c->prescalercounter = 0;
			c->counter++;
		}
		else {
			c->prescalercounter++;
		}

		if (c->counter == c->matcha) {
			if (MATCHA_RESET(c)) {
				c->counter = 0;
			}

			if (MATCHA_INTERRUPT_ENABLED(c) && !FLAGGED_INTERRUPTMATCHA(c)) {
				SET_FLAG_INTERRUPTMATCHA(c);
				c->cb->raiseinterrupt();
			}
		}

		if (c->counter == c->matchb) {
			if (MATCHB_RESET(c)) {
				c->counter = 0;
			}

			if (MATCHB_INTERRUPT_ENABLED(c) && !FLAGGED_INTERRUPTMATCHB(c)) {
				SET_FLAG_INTERRUPTMATCHB(c);
				c->cb->raiseinterrupt();
			}
		}
	}

}

#define GETREGISTER(a) ((a & 0xf) >> 1)

static void timer_dumpconfig(context_t* context) {

	log_println(LEVEL_INFO, TAG, "flags: 0x%04"PRIx16, context->flags);
	log_println(LEVEL_INFO, TAG, "config: 0x%04"PRIx16, context->config);
	log_println(LEVEL_INFO, TAG, "prescaler: 0x%04"PRIx16, context->prescaler);
	log_println(LEVEL_INFO, TAG, "prescalercounter: 0x%04"PRIx16, context->prescalercounter);
	log_println(LEVEL_INFO, TAG, "counter: 0x%04"PRIx16, context->counter);
	log_println(LEVEL_INFO, TAG, "matcha: 0x%04"PRIx16, context->matcha);
	log_println(LEVEL_INFO, TAG, "matchb: 0x%04"PRIx16, context->matchb);

}

static uint16_t* timer_getregisterincontext(void* context, uint16_t address) {
	context_t* c = (context_t*) context;
	switch (GETREGISTER(address)) {
		case 0x0:
			return &(c->flags);
		case 0x1:
			return &(c->config);
		case 0x2:
			return &(c->prescaler);
		case 0x3:
			return &(c->prescalercounter);
		case 0x4:
			return &(c->counter);
		case 0x5:
			return &(c->matcha);
		case 0x6:
			return &(c->matchb);

	}

	return NULL ;
}

static void timer_writeword(void* context, uint16_t address, uint16_t value) {
	context_t* c = (context_t*) context;
	uint16_t* reg = timer_getregisterincontext(context, address);
	if (reg != NULL ) {
		// this a fake write to clear the interrupt flags
		if (reg == &(c->flags)) {
			CLEAR_FLAG_INTERRUPTMATCHA(c);
			CLEAR_FLAG_INTERRUPTMATCHB(c);
		}
		else
			*reg = value;
	}

	timer_dumpconfig((context_t*) context);
}

static uint16_t timer_readword(void* context, uint16_t address) {
	uint16_t* reg = timer_getregisterincontext(context, address);
	if (reg != NULL )
		return *reg;
	else
		return 0;
}

const module timermodule = { //
		TAG, //
				timer_init, //
				NULL, //
				timer_tick, //
				NULL, //
				timer_readword, //
				NULL, //
				NULL, //
				timer_writeword, //
				NULL //
		};
