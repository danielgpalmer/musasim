/*
 * machine.c
 *
 *  Created on: Jan 19, 2012
 *      Author: daniel
 */

#include "include/machine.h"
#include "include/machine_stuff.h"

#include <stdint.h>

uint16_t machine_getstatusregister() {
	uint16_t temp;
	asm volatile (
			"move.w %%sr, %0\n\t"
			:
			: "g"(temp)
			: );
	return temp;
}

void machine_setstatusregister(uint16_t value) {
	asm volatile (
			"move.w %0, %%sr\n\t"
			:
			: "g"(value)
			: );
}

void machine_enableinterrupts() {

}

void machine_disableinterrupts() {

}

void machine_disablerom() {
	romdisable0 = ROMDISABLE0;
	romdisable1 = ROMDISABLE1;
}

void machine_reset() {
	asm volatile (
			"reset \n\t"
			:
			:
			: );
}
