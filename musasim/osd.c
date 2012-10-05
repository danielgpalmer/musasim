/*
 * osd.c
 *
 *  Created on: 16 Jun 2012
 *      Author: daniel
 */

#include <SDL/SDL.h>
#include <SDL_ttf.h>
#include <stdbool.h>
#include "osd.h"
#include "fontutils.h"
#include "hardware/cards/card.h"
#include "hardware/cards/videocard.h"
#include "hardware/cards/inputcard.h"
#include "hardware/board.h"

#define LEDHEIGHT  10
#define LEDWIDTH  15
#define LEDSPACING  5
#define LEDSTRIDE (LEDWIDTH + LEDSPACING)
#define LEDS 8

static TTF_Font* font = NULL;
static SDL_Surface* osd = NULL;
static SDL_Surface* debuglabel;
static SDL_Rect rect, rectled;
static SDL_Color labels = { .r = 0, .g = 0xff, .b = 0 };
static Uint32 colourkey, active, inactive, ledon, ledoff;
static bool osdvisible = false;

static void osd_set() {
	videocard_setosd(osdvisible ? osd : NULL );
}

void osd_createlabels() {
	TTF_Init();
	font = TTF_OpenFont(fontutils_getmonospace(), 12);
	debuglabel = TTF_RenderUTF8_Solid(font, "debug leds", labels);
	if (font != NULL )
		TTF_CloseFont(font);

	TTF_Quit();

}

void osd_init() {

	osd_createlabels();

	osd = SDL_CreateRGBSurface(SDL_SWSURFACE | SDL_ASYNCBLIT, VIDEO_WIDTH, VIDEO_HEIGHT, VIDEO_PIXELFORMAT, 0, 0, 0, 0);
	colourkey = SDL_MapRGB(osd->format, 0, 0, 0);
	active = SDL_MapRGB(osd->format, 0xff, 0, 0);
	inactive = SDL_MapRGB(osd->format, 184, 184, 184);
	ledon = SDL_MapRGB(osd->format, 0, 255, 0);
	ledoff = SDL_MapRGB(osd->format, 0, 100, 0);
	SDL_SetColorKey(osd, SDL_SRCCOLORKEY, colourkey);
	SDL_FillRect(osd, NULL, colourkey);

	rect.h = 10;
	rect.w = 20;
	rect.x = 0;
	rect.y = 10;

	rectled.h = LEDHEIGHT;
	rectled.w = LEDWIDTH;
	rectled.x = 0;
	rectled.y = 200;

	osd_set();

}

void osd_update() {

	for (int i = 0; i < NUM_SLOTS; i++) {
		rect.x = 10 + ((rect.w + 10) * i);

		Uint32 colour = inactive;
		const card* c = board_getcardinslot(i);

		if (c != NULL ) {
			if (c->active != NULL ) {
				if (c->active()) {
					colour = active;
				}
			}
		}

		SDL_FillRect(osd, &rect, colour);

	}

	SDL_BlitSurface(debuglabel, NULL, osd, NULL );

	uint8_t leds = inputcard_getleds();
	rectled.x = 0;
	for (int i = 0; i < LEDS; i++) {
		SDL_FillRect(osd, &rectled, ((leds >> i) & 1) ? ledon : ledoff);
		rectled.x += LEDSTRIDE;
	}
}

void osd_toggle() {
	osdvisible = !osdvisible;
	osd_set();
}

void osd_visible(bool visible) {
	osdvisible = visible;
	osd_set();
}

void osd_dispose() {
	SDL_FreeSurface(osd);

}
