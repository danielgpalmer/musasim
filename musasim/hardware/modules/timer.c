/*
 * timer.c
 *
 *  Created on: Jan 19, 2012
 *      Author: daniel
 */

#include <stdint.h>
#include "module.h"

#define CRYSTALFREQ 32767
#define TIMERCHANNELS 4

static uint16_t secondcounter = 0;
static uint32_t seconds = 0;

#define ONESECOND (1 << 15)

#define ALARM0_EN (1 << 6)
#define ALARM1_EN (1 << 7)

//	7	6	5	4	3	2	1	0
//	a1	a0  A1 A0
//
//  a1 - Alarm 0 Fired
//  a0 - Alarm 1 Fired
//	A1 - Enable Alarm 0
// 	A0 - Enable Alarm 1

static uint8_t flags = 0;

static uint32_t alarm0 = 0;
static uint32_t alarm1 = 0;

#define TIMER_EN ( 1 << 7)

typedef struct {
	//	7	6	5	4	3	2	1	0
	//								E
	// E - Enable
	uint8_t flags;
	uint32_t tickcounter;
	uint16_t divider;
	uint16_t count;
} timer;

static timer timers[TIMERCHANNELS];

void tick() {

	secondcounter++;
	if (secondcounter == ONESECOND) {
		secondcounter = 0;
		seconds++;
	}

	if ((flags & ALARM0_EN) && (seconds == alarm0)) {
		// fire int
	}

	if ((flags & ALARM1_EN) && (seconds == alarm1)) {
		// fire int
	}

	for (int i = 0; i < TIMERCHANNELS; i++) {
		if (timers[i].flags & TIMER_EN) {
			timers[i].tickcounter++;
			if (timers[i].tickcounter == timers[i].divider) {
				timers[i].count++;
			}
		}
	}

}
