/*
 * main.c
 *
 *  Created on: 22 Sep 2012
 *      Author: daniel
 */

#include <inttypes.h>
#include <stdio.h>
#include <libunagipai/machine.h>
#include <libunagipai/timers.h>
#include <libunagipai/timers_registers.h>
#include <libunagipai/timers_registermasks.h>
#include <libunagipai/input_registers.h>

void timerinterrupt() __attribute__ (( interrupt ));
void timerinterrupt() {

	//printf("timer interrupt 0x%"PRIx16"!\n", timers_timerinterrupts);

	if (timers_timerinterrupts & 1) {

		if (timers_timer_0_flags & TIMERS_REGISTER_FLAGS_MATCHAINT) {
			//printf("match a\n");
			input_debugleds |= 0x1;

		}

		else if (timers_timer_0_flags & TIMERS_REGISTER_FLAGS_MATCHBINT) {
			//printf("match b\n");
			input_debugleds &= ~0x1;
		}

		timers_timer_0_flags = 0x00;
	}

	if (timers_timerinterrupts & 0x10) {
		if (timers_bigtimer_0_flags & TIMERS_REGISTER_FLAGS_MATCHAINT) {
			//printf("big match a\n");
			input_debugleds |= 0x2;

		}

		else if (timers_bigtimer_0_flags & TIMERS_REGISTER_FLAGS_MATCHBINT) {
			//printf("big match b\n");
			input_debugleds &= ~0x2;
		}
		timers_bigtimer_0_flags = 0x00;
	}

}

int main() {

	timers_setup(0,
			TIMERS_REGISTER_CONFIG_ENABLE | TIMERS_REGISTER_CONFIG_ENMATCHAINT | TIMERS_REGISTER_CONFIG_ENMATCHBINT
					| TIMERS_REGISTER_CONFIG_RESETMATCHB, 0xffff, 0x00ff, 0x01ff);
	timers_bigsetup(0,
			TIMERS_REGISTER_CONFIG_ENABLE | TIMERS_REGISTER_CONFIG_ENMATCHAINT | TIMERS_REGISTER_CONFIG_ENMATCHBINT
					| TIMERS_REGISTER_CONFIG_RESETMATCHB, 0xffff, 0xff0, 0x1ff0);

	uint16_t sr = machine_getstatusregister();
	machine_setstatusregister((sr & 0xf8ff));
	while (1) {

	}

}
