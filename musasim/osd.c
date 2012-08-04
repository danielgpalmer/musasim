/*
 * osd.c
 *
 *  Created on: 16 Jun 2012
 *      Author: daniel
 */

#include <SDL/SDL.h>
#include <stdbool.h>
#include "osd.h"
#include "hardware/cards/card.h"
#include "hardware/cards/videocard.h"
#include "hardware/board.h"

static SDL_Surface* osd = NULL;
static SDL_Rect rect;
static Uint32 colourkey, active, inactive;

void osd_init() {
	osd = SDL_CreateRGBSurface(SDL_SWSURFACE | SDL_ASYNCBLIT, VIDEO_WIDTH, VIDEO_HEIGHT, VIDEO_PIXELFORMAT, 0, 0, 0, 0);
	colourkey = SDL_MapRGB(osd->format, 0, 0, 0);
	active = SDL_MapRGB(osd->format, 0xff, 0, 0);
	inactive = SDL_MapRGB(osd->format, 184, 184, 184);
	SDL_SetColorKey(osd, SDL_SRCCOLORKEY, colourkey);
	SDL_FillRect(osd, NULL, colourkey);

	rect.h = 10;
	rect.w = 20;
	rect.x = 0;
	rect.y = 10;

	videocard_setosd(osd);

}

void osd_update() {

	for (int i = 0; i < NUM_SLOTS; i++) {
		rect.x = 10 + ((rect.w + 10) * i);

		Uint32 colour = inactive;
		const card* c = board_getcardinslot(i);

		if (c != NULL) {
			if (c->active != NULL) {
				if (c->active()) {
					colour = active;
				}
			}
		}

		SDL_FillRect(osd, &rect, colour);

	}
}

void osd_visible(bool visible) {

}

void osd_dispose() {
	SDL_FreeSurface(osd);
}
