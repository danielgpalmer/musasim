/*
 * main.c
 *
 *  Created on: 22 Sep 2012
 *      Author: daniel
 */

#include <libunagipai/machine.h>
#include <libunagipai/timers.h>

int main() {

	timers_setup();

	uint16_t sr = machine_getstatusregister();
	machine_setstatusregister((sr & 0xf8ff));
	while (1) {

	}

}
