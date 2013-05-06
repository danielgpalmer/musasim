/*
 * renderer.c
 *
 *  Created on: 6 May 2013
 *      Author: daniel
 */

#include <SDL/SDL.h>
#include <stdbool.h>
#include "logging.h"
#include "osd.h"
#include "hardware/cards/videocard.h"

#include "renderer.h"

static const char TAG[] = "renderer";
static const char WINDOWTITLE[] = "musasim";

static bool renderrequested = false;

static SDL_Surface* screen = NULL;

void renderer_init() {
	SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO | SDL_INIT_NOPARACHUTE);
	SDL_WM_SetCaption(WINDOWTITLE, WINDOWTITLE);
	screen = SDL_SetVideoMode(VIDEO_WIDTH, VIDEO_HEIGHT, 0, SDL_HWSURFACE);

	log_println(LEVEL_INFO, TAG, "Created surface; %d x %d pixels @ %dBPP", screen->w, screen->h,
			screen->format->BitsPerPixel);
}

void renderer_requestrender() {
	renderrequested = true;
}

void renderer_render() {
	if (renderrequested) {
		videocard_render(screen);
		osd_render(screen);
		SDL_Flip(screen);
		renderrequested = false;
	}
}
