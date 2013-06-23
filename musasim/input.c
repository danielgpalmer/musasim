/*
 * input.c
 *
 *  Created on: 6 May 2013
 *      Author: daniel
 */

#include "input.h"
#include "sim.h"
#include "logging.h"
#include <SDL.h>
#include "musashi/m68k.h"
#include "osd.h"
#include "throttler.h"
#include "hardware/cards/inputcard.h"

static const char TAG[] = "input";

// keys that the sim uses
#define SIM_KEY_PAUSE			SDLK_F1
#define SIM_KEY_RESET			SDLK_F2
#define SIM_KEY_NMI				SDLK_F3
#define SIM_KEY_MUTE			SDLK_F4
#define SIM_KEY_TOGGLEOSD		SDLK_F5
#define SIM_KEY_TOGGLETHROTTLE	SDLK_F6
#define SIM_KEY_QUIT			SDLK_ESCAPE

void input_init() {
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

void input_update() {

	// Check some keys
	SDL_PumpEvents();
	static SDL_Event event;
	if (SDL_PeepEvents(&event, 1, SDL_GETEVENT, SDL_QUITMASK)) {
		log_println(LEVEL_INFO, TAG, "Window was closed");
		sim_quit();
		return;
	}

	// pick out the events that the sim wants
	while (SDL_PollEvent(&event)) {
		if (event.type == SDL_KEYUP) {
			switch (event.key.keysym.sym) {
				case SIM_KEY_NMI:
					m68k_set_irq(7);
					break;
				case SIM_KEY_RESET:
					break;
				case SIM_KEY_MUTE:
					break;
				case SIM_KEY_TOGGLETHROTTLE:
					throttler_toggle();
					break;
				case SIM_KEY_TOGGLEOSD:
					log_println(LEVEL_INFO, TAG, "toggling osd");
					osd_toggle();
					break;
				case SIM_KEY_PAUSE:
					sim_togglepause();
					return;
				case SIM_KEY_QUIT:
					sim_quit();
					return;
				default:
					inputcard_onkeyevent(&event);
					break;
			}
		}
		else if (event.type == SDL_KEYDOWN) {
			switch (event.key.keysym.sym) {
				case SIM_KEY_NMI:
				case SIM_KEY_RESET:
				case SIM_KEY_MUTE:
				case SIM_KEY_TOGGLETHROTTLE:
				case SIM_KEY_TOGGLEOSD:
				case SIM_KEY_PAUSE:
				case SIM_KEY_QUIT:
					return;
				default:
					inputcard_onkeyevent(&event);
					break;
			}
		}
	}

}
