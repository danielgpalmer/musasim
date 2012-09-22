/*
 * timers.c
 *
 *  Created on: 22 Sep 2012
 *      Author: daniel
 */

#include "include/timers_registers.h"

void timers_setup() {

	timers_timer_0_config |= 0x1;
	timers_timer_0_prescaler = 0x0f;
	timers_timer_0_matcha = 0xff;

}

