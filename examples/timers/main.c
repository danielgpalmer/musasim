/*
 * main.c
 *
 *  Created on: 22 Sep 2012
 *      Author: daniel
 */

#include <libunagipai/machine.h>
#include <libunagipai/timers.h>
#include <libunagipai/timers_registers.h>
#include <libunagipai/timers_registermasks.h>
#include <libunagipai/input_registers.h>

void timerinterrupt() __attribute__ (( interrupt ));
void timerinterrupt() {

	printf("timer interrupt!\n");
	if (TIMERS_INTERRUPTFIRED_MATCHA) {
		printf("match a\n");
		input_debugleds = 0x1;

	}

	if (TIMERS_INTERRUPTFIRED_MATCHB) {
		printf("match b\n");
		input_debugleds = 0;
	}

	TIMERS_CLEARFLAGS;

}

int main() {

	timers_setup();
	timers_bigsetup();

	uint16_t sr = machine_getstatusregister();
	//machine_setstatusregister((sr & 0xf8ff));
	while (1) {

	}

}
