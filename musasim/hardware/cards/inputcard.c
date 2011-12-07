/*
 * inputcard.c
 *
 *  Created on: Dec 3, 2011
 *      Author: daniel
 */

#include "inputcard.h"

// Basically a megadrive pad..

#define PORT0_UP	'w'
#define PORT0_DOWN	'z'
#define PORT0_LEFT	'a'
#define PORT0_RIGHT	's'
#define PORT0_A 	'j'
#define PORT0_B		'k'
#define PORT0_C		'l'
#define PORT0_START 0x20 // Space
//	7	|	6	|	5	|	4	|	3	|	2	|	1	|	0
//	U		D		L		R		A		B		C		S
// Active Low

#define BUTTON_UP		0x7f
#define BUTTON_DOWN 	0xbf
#define BUTTON_LEFT 	0xdf
#define BUTTON_RIGHT	0xef
#define BUTTON_A		0xf7
#define BUTTON_B		0xfb
#define BUTTON_C		0xfd
#define BUTTON_START	0xfe

uint8_t ports[2];

void inputcard_init() {
	for (int i = 0; i < sizeof(ports); i++) {
		ports[i] = 0;
	}
}

uint8_t inputcard_read_byte(uint32_t address) {

	int port = address & 0x1;
	return ports[port];

}

void inputcard_tick() {
	// "latch" stuff here

	for (int i = 0; i < sizeof(ports); i++) {
		// do port things here
	}

}

card inputcard = { "INPUT CARD", inputcard_init, NULL, inputcard_tick, NULL, NULL, inputcard_read_byte, NULL, NULL,
		NULL, NULL, NULL };
