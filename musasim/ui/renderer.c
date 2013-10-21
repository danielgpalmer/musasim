/*
 * renderer.c
 *
 *  Created on: 6 May 2013
 *      Author: daniel
 */

#include <SDL.h>
#include <stdbool.h>
#include "logging.h"
#include "osd.h"
#include "hardware/cards/videocard.h"

#include "renderer.h"

static const char TAG[] = "renderer";
static const char WINDOWTITLE[] = "musasim";

static bool renderrequested = false;

static SDL_Window* window = NULL;
static SDL_Renderer* renderer = NULL;
static SDL_Surface *screen = NULL;
static SDL_Texture *sdlTexture = NULL;

void renderer_init() {

	SDL_CreateWindowAndRenderer(VIDEO_WIDTH, VIDEO_HEIGHT, 0, &window, &renderer);
	screen = SDL_CreateRGBSurface(0, VIDEO_WIDTH, VIDEO_HEIGHT,
	VIDEO_PIXELFORMAT, 0, 0, 0, 0);
	if (screen == NULL)
		log_println(LEVEL_WARNING, TAG, "Failed to create surface");

	//screen = SDL_CreateRGBSurface(0, VIDEO_WIDTH, VIDEO_HEIGHT, VIDEO_PIXELFORMAT, 0, 0, 0, 0);
	sdlTexture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGB565, SDL_TEXTUREACCESS_STREAMING, 640, 480);
	//SDL_WM_SetCaption(WINDOWTITLE, WINDOWTITLE);
	//screen = SDL_SetVideoMode(VIDEO_WIDTH, VIDEO_HEIGHT, 0, SDL_HWSURFACE);
//
	//log_println(LEVEL_INFO, TAG, "Created surface; %d x %d pixels @ %dBPP", screen->w, screen->h,
	//		screen->format->BitsPerPixel);
}

void renderer_requestrender() {
	renderrequested = true;
}

void renderer_render() {
	if (renderrequested) {
		videocard_render(screen);
		//osd_render(screen);
		SDL_UpdateTexture(sdlTexture, NULL, screen->pixels, screen->pitch);
		SDL_SetRenderDrawColor(renderer, 0xff, 0xff, 0xff, 255);
		SDL_RenderClear(renderer);
		SDL_RenderCopy(renderer, sdlTexture, NULL, NULL);
		SDL_RenderPresent(renderer);
		renderrequested = false;
	}
}
