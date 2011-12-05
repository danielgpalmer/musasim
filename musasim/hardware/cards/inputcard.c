/*
 * inputcard.c
 *
 *  Created on: Dec 3, 2011
 *      Author: daniel
 */

#include "inputcard.h"

uint8_t port0 = 0xff;
uint8_t port1 = 0xff;

uint8_t inputcard_read_byte(uint32_t address) {

	int port = address & 0x1;

	switch (port) {
		case 0x0:
			return port0;
		case 0x1:
			return port1;
		default:
			return 0;
	}

}

void inputcard_tick() {
	// "latch" stuff here
}

card inputcard = { "INPUT CARD", NULL, NULL, inputcard_tick, inputcard_read_byte, NULL, NULL, NULL, NULL, NULL };
