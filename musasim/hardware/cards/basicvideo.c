/*
 * basicvideo.c
 *
 *  Created on: 21 May 2012
 *      Author: daniel
 */

#include "basicvideo.h"
#include <SDL/SDL.h>
#include "../../logging.h"
#include "../board.h"
#include <inttypes.h>

static const char TAG[] = "basicvideo";

static SDL_Surface* screen = NULL;

static void video_init() {

	log_println(LEVEL_DEBUG, TAG, "video_init()");

	screen = SDL_SetVideoMode(BASIC_VIDEO_WIDTH, BASIC_VIDEO_HEIGHT, BASIC_VIDEO_PIXELFORMAT, SDL_SWSURFACE);

	log_println(LEVEL_INFO, TAG, "Created surface; %d x %d pixels @ %dBPP", screen->w, screen->h,
			screen->format->BitsPerPixel);

}

static void video_dispose() {
}

static bool video_validaddress(uint32_t address) {
	return address < BASIC_VIDEO_WIDTH * BASIC_VIDEO_HEIGHT;
}

static void video_tick() {
	SDL_Flip(screen);
}

static void video_write_word(uint32_t address, uint16_t data) {

	log_println(LEVEL_DEBUG, TAG, "wrote 0x%04"PRIx16" to 0x%08"PRIx32, data, address);
	if (SDL_MUSTLOCK(screen)) {
		SDL_LockSurface(screen);
	}
	*((uint16_t*) screen->pixels + (address / 2)) = data;
	if (SDL_MUSTLOCK(screen)) {
		SDL_UnlockSurface(screen);
	}
}

static uint16_t video_read_word(uint32_t address) {
	return 0;
}

static void videocard_irqack() {
	board_lower_interrupt(&basicvideocard);
}

const card basicvideocard = { "BASIC VIDEO CARD", video_init, video_dispose, NULL, video_tick, videocard_irqack, NULL,
		video_validaddress, NULL, video_read_word, NULL, NULL, video_write_word, NULL };

