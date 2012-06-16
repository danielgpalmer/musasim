/*
 * machine.c
 *
 *  Created on: Jan 19, 2012
 *      Author: daniel
 */

#include "machine.h"
#include "machine_stuff.h"

#include <stdint.h>

uint16_t machine_getstatusregister() {
	volatile uint16_t temp; // stops this guy getting optimised out
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

void machine_switchtousermode() {
	uint16_t sr = machine_getstatusregister();
	sr |= MACHINE_SR_MASK;
	machine_setstatusregister(sr);
}

void machine_disablerom() {
	romdisable0 = ROMDISABLE0;
	romdisable1 = ROMDISABLE1;
}

void machine_reset() {
	asm volatile (
			"reset \n\t"
			"lea.l 0, %a0 \n\t"
			"move.l (%a0)+, %sp \n\t"
			"jmp (%a0)\n\t");
}
