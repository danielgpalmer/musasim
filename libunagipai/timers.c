/*
 * timers.c
 *
 *  Created on: 22 Sep 2012
 *      Author: daniel
 */

#include "include/timers.h"
#include "include/timers_registers.h"
#include "include/timers_registermasks.h"

void timers_setup() {

	timers_timer_0_config |= TIMERS_REGISTER_CONFIG_ENABLE | TIMERS_REGISTER_CONFIG_ENMATCHAINT
			| TIMERS_REGISTER_CONFIG_ENMATCHBINT | TIMERS_REGISTER_CONFIG_RESETMATCHB;
	timers_timer_0_prescaler = 0xffff;
	timers_timer_0_matcha = 0x00ff;
	timers_timer_0_matchb = 0x01ff;
}

void timers_bigsetup() {
	timers_bigtimer_0_config |= TIMERS_REGISTER_CONFIG_ENABLE | TIMERS_REGISTER_CONFIG_ENMATCHAINT
			| TIMERS_REGISTER_CONFIG_ENMATCHBINT | TIMERS_REGISTER_CONFIG_RESETMATCHB;
	timers_bigtimer_0_prescaler = 0xffff;
	timers_bigtimer_0_matcha = 0x00ff;
	timers_bigtimer_0_matchb = 0x01ff;
}
