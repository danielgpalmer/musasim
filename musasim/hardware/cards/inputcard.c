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

static void inputcard_init() {

	log_println(LEVEL_DEBUG, TAG, "inputcard_init()");

	for (int i = 0; i < sizeof(ports); i++) {
		ports[i] = 0xFF;
	}

	// Ignore all the events that aren't needed
	SDL_EventState(SDL_ACTIVEEVENT, SDL_IGNORE);
	SDL_EventState(SDL_MOUSEMOTION, SDL_IGNORE);
	SDL_EventState(SDL_MOUSEBUTTONDOWN, SDL_IGNORE);
	SDL_EventState(SDL_MOUSEBUTTONUP, SDL_IGNORE);
	SDL_EventState(SDL_JOYAXISMOTION, SDL_IGNORE);
	SDL_EventState(SDL_JOYBALLMOTION, SDL_IGNORE);
	SDL_EventState(SDL_JOYHATMOTION, SDL_IGNORE);
	SDL_EventState(SDL_JOYBUTTONDOWN, SDL_IGNORE);
	SDL_EventState(SDL_JOYBUTTONUP, SDL_IGNORE);

}

static uint8_t inputcard_read_byte(uint32_t address) {
	log_println(LEVEL_INSANE, TAG, "inputcard_read_byte()");

	int reg = address & 0x3;

	switch (reg) {
		case INPUT_PORT0:
			return ports[0];
		case INPUT_PORT1:
			return ports[1];
		case INPUT_RNG:
			return rand() & 0xff;
	}

	return 0;

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

static void inputcard_tick(int cyclesexecuted) {
	// "latch" stuff here

	static SDL_Event events[10];

	for (int i = 0; i < SDL_PeepEvents(events, 10, SDL_GETEVENT, SDL_KEYDOWNMASK | SDL_KEYUPMASK); i++) {
		switch (events[i].type) {
			case SDL_KEYDOWN:
				log_println(LEVEL_DEBUG, TAG, "key down");
				inputcard_decodekey(events[i].key.keysym.sym, false);
				break;
			case SDL_KEYUP:
				log_println(LEVEL_DEBUG, TAG, "key up");
				inputcard_decodekey(events[i].key.keysym.sym, true);
				break;
		}
	}

	for (int i = 0; i < sizeof(ports); i++) {
		// do port things here
	}

}

static bool input_validaddress(uint32_t address) {
	int reg = address & 0x3;
	switch (reg) {
		case INPUT_PORT0:
		case INPUT_PORT1:
		case INPUT_RNG:
			return true;
		default:
			return false;
	}
}

const card inputcard = { "INPUT CARD", // tag
		inputcard_init, // init
		NULL, // dispose
		NULL, // reset
		inputcard_tick, //tick
		NULL, // IRQ ack
		NULL, // BUSRQ ack
		NULL, //
		input_validaddress, //valid address
		inputcard_read_byte, // read byte
		NULL, // read word
		NULL, // read long
		NULL, // write byte
		NULL, // write word
		NULL, // write long
		NULL, //
		NULL, //
		NULL };
