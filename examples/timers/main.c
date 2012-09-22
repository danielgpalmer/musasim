/*
 * main.c
 *
 *  Created on: 22 Sep 2012
 *      Author: daniel
 */

#include <libunagipai/machine.h>
#include <libunagipai/timers.h>

void interrupthandler1() __attribute__ (( interrupt ));
void interrupthandler1() {
}

void interrupthandler2() __attribute__ (( interrupt ));
void interrupthandler2() {
}

void interrupthandler3() __attribute__ (( interrupt ));
void interrupthandler3() {
}

void interrupthandler4() __attribute__ (( interrupt ));
void interrupthandler4() {
}

void interrupthandler5() __attribute__ (( interrupt ));
void interrupthandler5() {
}

void interrupthandler6() __attribute__ (( interrupt ));
void interrupthandler6() {
}

void interrupthandler7() __attribute__ (( interrupt ));
void interrupthandler7() {
}

int main() {

	timers_setup();

	uint16_t sr = machine_getstatusregister();
	machine_setstatusregister((sr & 0xf8ff));
	while (1) {

	}

}
