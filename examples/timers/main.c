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

	timers_setup();
	timers_bigsetup();

	uint16_t sr = machine_getstatusregister();
	machine_setstatusregister((sr & 0xf8ff));
	while (1) {

	}

}
