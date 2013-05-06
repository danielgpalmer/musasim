/*
 * inputcard.c
 *
 *  Created on: Dec 3, 2011
 *      Author: daniel
 */

#include <stdio.h>
#include <SDL/SDL.h>
#include <stdbool.h>
#include <stdlib.h>
#include <input_registermasks.h>
#include "inputcard.h"
#include "../../sim.h"
#include "../../logging.h"

static const char TAG[] = "input";

// Basically a megadrive pad..

#define PORT0_UP	SDLK_w
#define PORT0_DOWN	SDLK_z
#define PORT0_LEFT	SDLK_a
#define PORT0_RIGHT	SDLK_s
#define PORT0_A 	SDLK_j
#define PORT0_B		SDLK_k
#define PORT0_C		SDLK_l
#define PORT0_START SDLK_SPACE // Space
//	7	|	6	|	5	|	4	|	3	|	2	|	1	|	0
//	U		D		L		R		A		B		C		S
// Active Low

static uint8_t ports[2];
static uint8_t dips;
static uint8_t debugleds = 0;

static void inputcard_init() {

	log_println(LEVEL_DEBUG, TAG, "inputcard_init()");

	for (int i = 0; i < sizeof(ports); i++) {
		ports[i] = 0xFF;
	}

}

static uint8_t inputcard_read_byte(uint32_t address) {
	log_println(LEVEL_INSANE, TAG, "inputcard_read_byte()");

	int reg = address & 0xf;

	switch (reg) {
		case INPUT_PORT0:
			return ports[0];
		case INPUT_PORT1:
			return ports[1];
		case INPUT_RNG:
			//todo in GDBSERVER mode this should be deterministic.
			return rand() & 0xff;
		case INPUT_DIPS:
			return dips;
		case INPUT_DEBUGLEDS:
			return debugleds;
	}

	return 0;

}

static void inputcard_write_byte(uint32_t address, uint8_t value) {
	int reg = address & 0xf;
	if (reg == INPUT_DEBUGLEDS)
		debugleds = value;
}

static void inputcard_decodekey(SDLKey key, bool up) {

	int port = 0;
	uint8_t mask;

	switch (key) {
		case PORT0_UP:
			log_println(LEVEL_DEBUG, TAG, "Button UP");
			port = 0;
			mask = BUTTON_UP;
			break;
		case PORT0_DOWN:
			log_println(LEVEL_DEBUG, TAG, "Button DOWN");
			port = 0;
			mask = BUTTON_DOWN;
			break;
		case PORT0_LEFT:
			log_println(LEVEL_DEBUG, TAG, "Button LEFT");
			port = 0;
			mask = BUTTON_LEFT;
			break;
		case PORT0_RIGHT:
			log_println(LEVEL_DEBUG, TAG, "Button RIGHT");
			port = 0;
			mask = BUTTON_RIGHT;
			break;
		case PORT0_START:
			log_println(LEVEL_DEBUG, TAG, "Button START");
			port = 0;
			mask = BUTTON_START;
			break;
		case PORT0_A:
			log_println(LEVEL_DEBUG, TAG, "Button A");
			port = 0;
			mask = BUTTON_A;
			break;
		case PORT0_B:
			log_println(LEVEL_DEBUG, TAG, "Button B");
			port = 0;
			mask = BUTTON_B;
			break;
		case PORT0_C:
			log_println(LEVEL_DEBUG, TAG, "Button C");
			port = 0;
			mask = BUTTON_C;
			break;
		default:
			log_println(LEVEL_DEBUG, TAG, "Unhandled key");
			return;
	}

	if (up) {
		ports[port] |= ~mask;
	}
	else {
		ports[port] &= mask;
	}
}

static void inputcard_tick(int cyclesexecuted, bool behind) __attribute__((hot));
static void inputcard_tick(int cyclesexecuted, bool behind) {
}

void inputcard_onkeyevent(SDL_Event* event) {
	switch (event->type) {
		case SDL_KEYDOWN:
			log_println(LEVEL_DEBUG, TAG, "key down");
			inputcard_decodekey(event->key.keysym.sym, false);
			break;
		case SDL_KEYUP:
			log_println(LEVEL_DEBUG, TAG, "key up");
			inputcard_decodekey(event->key.keysym.sym, true);
			break;
	}
}

static const bool input_validaddress(uint32_t address) {
	int reg = address & 0x3;
	switch (reg) {
		case INPUT_PORT0:
		case INPUT_PORT1:
		case INPUT_RNG:
		case INPUT_DIPS:
		case INPUT_DEBUGLEDS:
			return true;
		default:
			return false;
	}
}

static void inputcard_dispose() {

}

static void inputcard_reset() {
	debugleds = 0;
}

void inputcard_setdips(uint8_t dipsvalue) {
	dips = dipsvalue;
}

uint8_t inputcard_getleds() {
	return debugleds;
}

const card inputcard = { //
		"INPUT CARD", // tag
				inputcard_init, // init
				inputcard_dispose, // dispose
				inputcard_reset, // reset
				inputcard_tick, //tick
				NULL, // pause
				NULL, // IRQ ack
				NULL, // BUSRQ ack
				NULL, //
				input_validaddress, //valid address
				NULL, //
				inputcard_read_byte, // read byte
				NULL, // read word
				NULL, // read long
				inputcard_write_byte, // write byte
				NULL, // write word
				NULL, // write long
				NULL, //
				NULL, //
				NULL, //
				NULL //
		};
