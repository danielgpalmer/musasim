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

void timerinterrupt() __attribute__ (( interrupt ));
void timerinterrupt() {

	printf("timer interrupt!\n");
	if (TIMERS_INTERRUPTFIRED_MATCHA) {
		printf("match a\n");
	}

	if (TIMERS_INTERRUPTFIRED_MATCHB) {
		printf("match b\n");
	}

	TIMERS_CLEARFLAGS;

}

int main() {

	timers_setup();

	uint16_t sr = machine_getstatusregister();
	machine_setstatusregister((sr & 0xf8ff));
	while (1) {

	}

}
