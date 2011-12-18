/*
 * inputcard.c
 *
 *  Created on: Dec 3, 2011
 *      Author: daniel
 */

#include <stdio.h>
#include <SDL/SDL.h>
#include <stdbool.h>
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

	log_println(LEVEL_DEBUG, TAG, "inputcard_init()");

	for (int i = 0; i < sizeof(ports); i++) {
		ports[i] = 0;
	}

}

uint8_t inputcard_read_byte(uint32_t address) {
	log_println(LEVEL_DEBUG, TAG, "inputcard_read_byte()");
	int port = address & 0x1;
	return ports[port];

}

void inputcard_decodekey(SDLKey key, bool up) {

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
			mask = BUTTON_UP;
			break;
		case PORT0_LEFT:
			log_println(LEVEL_DEBUG, TAG, "Button LEFT");
			port = 0;
			mask = BUTTON_UP;
			break;
		case PORT0_RIGHT:
			log_println(LEVEL_DEBUG, TAG, "Button RIGHT");
			port = 0;
			mask = BUTTON_UP;
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
		ports[port] &= ~mask;
	}
	else {
		ports[port] |= mask;
	}
}

void inputcard_tick() {
	// "latch" stuff here

	SDL_Event event;

	while (SDL_PollEvent(&event)) {
		switch (event.type) {
			case SDL_QUIT:
				log_println(LEVEL_INFO, TAG, "Window was closed");
				sim_quit();
				break;
			case SDL_KEYDOWN:
				log_println(LEVEL_DEBUG, TAG, "key down");
				inputcard_decodekey(event.key.keysym.sym, false);
				break;
			case SDL_KEYUP:
				log_println(LEVEL_DEBUG, TAG, "key up");
				inputcard_decodekey(event.key.keysym.sym, true);
				break;
		}
	}

	for (int i = 0; i < sizeof(ports); i++) {
		// do port things here
	}

}

card inputcard = { "INPUT CARD", inputcard_init, NULL, inputcard_tick, NULL, NULL, inputcard_read_byte, NULL, NULL,
		NULL, NULL, NULL };
