/*
 * timers.c
 *
 *  Created on: 22 Sep 2012
 *      Author: daniel
 */

#include <stdint.h>

#include "include/timers.h"
#include "include/timers_registers.h"
#include "include/timers_registermasks.h"

void timers_setup(int which, uint16_t config, uint16_t prescaler, uint16_t matcha, uint16_t matchb) {

	switch (which) {
		case 0:
			timers_timer_0_config = config;
			timers_timer_0_prescaler = prescaler;
			timers_timer_0_matcha = matcha;
			timers_timer_0_matchb = matchb;
			break;
		case 1:
			timers_timer_1_config = config;
			timers_timer_1_prescaler = prescaler;
			timers_timer_1_matcha = matcha;
			timers_timer_1_matchb = matchb;
			break;
		case 2:
			timers_timer_2_config = config;
			timers_timer_2_prescaler = prescaler;
			timers_timer_2_matcha = matcha;
			timers_timer_2_matchb = matchb;
			break;
		case 3:
			timers_timer_3_config = config;
			timers_timer_3_prescaler = prescaler;
			timers_timer_3_matcha = matcha;
			timers_timer_3_matchb = matchb;
			break;
	}

}

void timers_bigsetup(int which, uint16_t config, uint32_t prescaler, uint32_t matcha, uint32_t matchb) {
	switch (which) {
		case 0:
			timers_bigtimer_0_config = config;
			timers_bigtimer_0_prescaler = prescaler;
			timers_bigtimer_0_matcha = matcha;
			timers_bigtimer_0_matchb = matchb;
			break;
		case 1:
			timers_bigtimer_1_config = config;
			timers_bigtimer_1_prescaler = prescaler;
			timers_bigtimer_1_matcha = matcha;
			timers_bigtimer_1_matchb = matchb;
			break;
		case 2:
			timers_bigtimer_2_config = config;
			timers_bigtimer_2_prescaler = prescaler;
			timers_bigtimer_2_matcha = matcha;
			timers_bigtimer_2_matchb = matchb;
			break;
		case 3:
			timers_bigtimer_3_config = config;
			timers_bigtimer_3_prescaler = prescaler;
			timers_bigtimer_3_matcha = matcha;
			timers_bigtimer_3_matchb = matchb;
			break;
	}
}
