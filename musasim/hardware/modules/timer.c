/*
 * timer.c
 *
 *  Created on: Jan 19, 2012
 *      Author: daniel
 */

#include <stdint.h>
#include <stddef.h>
#include <stdlib.h>
#include "timer.h"
#include "module.h"

typedef struct {
	module_callback* cb;
	uint16_t flags;
	uint16_t config;
	uint16_t precaler;
	uint16_t counter;
	uint16_t matcha;
	uint16_t matchb;
} context_t;

static void* timer_init(module_callback* callback) {
	context_t * c = malloc(sizeof(context_t));
	c->cb = callback;
	return c;
}

static void timer_tick(void* context) {

	context_t* c = (context_t*) context;
	c->counter++;

}

static void timer_writeword(void* context, uint16_t address, uint16_t value) {

}

static uint16_t timer_readword(void* context, uint16_t address) {
	return 0;
}

const module timermodule = { //
		timer_init, //
				NULL, //
				timer_tick, //
				NULL, //
				timer_readword, //
				NULL, //
				NULL, //
				timer_writeword, //
				NULL };
