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
static SDL_Surface* busactivitylabel;
static SDL_Surface* dipswitcheslabel;
static SDL_Surface* debuglabel;
static SDL_Rect rect, ledlabel, rectled;
static SDL_Color labels = { .r = 0, .g = 0xff, .b = 0 };
static Uint32 colourkey, active, inactive, ledon, ledoff;
static bool osdvisible = false;

static void osd_set() {
	videocard_setosd(osdvisible ? osd : NULL );
}

void osd_createlabels() {
	TTF_Init();
	font = TTF_OpenFont(fontutils_getmonospace(), 15);
	if (font != NULL ) {
		busactivitylabel = TTF_RenderUTF8_Solid(font, "bus activity", labels);
		dipswitcheslabel = TTF_RenderUTF8_Solid(font, "dip switches", labels);
		debuglabel = TTF_RenderUTF8_Solid(font, "debug leds", labels);
		TTF_CloseFont(font);
	}
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
	rect.w = LEDWIDTH;
	rect.x = 0;
	rect.y = 10;

	rectled.h = LEDHEIGHT;
	rectled.w = LEDWIDTH;
	rectled.x = 0;
	rectled.y = VIDEO_HEIGHT - LEDHEIGHT - 10;

	osd_set();

}

void osd_update() {

	for (int i = 0; i < NUM_SLOTS; i++) {
		rect.x = LEDSPACING + ((rect.w + LEDSPACING) * i);

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

	SDL_BlitSurface(busactivitylabel, NULL, osd, NULL );
	SDL_BlitSurface(dipswitcheslabel, NULL, osd, NULL );

	uint8_t leds = inputcard_getleds();
	rectled.x = 10;
	for (int i = 0; i < LEDS; i++) {
		SDL_FillRect(osd, &rectled, ((leds >> i) & 1) ? ledon : ledoff);
		rectled.x += LEDSTRIDE;
	}

	ledlabel.x = 10;
	ledlabel.y = rectled.y - debuglabel->h - 5;
	ledlabel.w = debuglabel->w;
	ledlabel.h = debuglabel->h;
	SDL_BlitSurface(debuglabel, NULL, osd, &ledlabel);
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
	SDL_FreeSurface(debuglabel);
	SDL_FreeSurface(busactivitylabel);
	SDL_FreeSurface(dipswitcheslabel);
	SDL_FreeSurface(osd);

}
