/*
 * osd.c
 *
 *  Created on: 16 Jun 2012
 *      Author: daniel
 */

#include <SDL/SDL.h>
#include <SDL_ttf.h>
#include <stdbool.h>
#include <glib.h>
#include "osd.h"
#include "sdlwrapper.h"
#include "fontutils.h"
#include "logging.h"
#include "hardware/board.h"
#include "hardware/cards/card.h"
#include "hardware/cards/videocard.h"
#include "hardware/cards/inputcard.h"
#include "hardware/cards/soundcard.h"

#define TAG "osd"

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
static SDL_Rect rect, ledlabel, rectled, audiowindow;
static SDL_Color labels = { .r = 0, .g = 0xff, .b = 0 };
static Uint32 colourkey, active, inactive, ledon, ledoff, audiowindowbg;
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

static void osd_updateaudiobuffer() {
	SDL_FillRect(osd, &audiowindow, audiowindowbg);
	int quarterheight = audiowindow.h / 4;
	int middle = audiowindow.y + quarterheight * 2;
	int leftbase = audiowindow.y + quarterheight;
	int rightbase = audiowindow.y + (quarterheight * 3);

	int windowright = audiowindow.x + audiowindow.w - 1;
	sdlwrapper_drawline(osd, audiowindow.x, leftbase, windowright, leftbase, 0, 0xFF0000FF);
	sdlwrapper_drawline(osd, audiowindow.x, rightbase, windowright, rightbase, 0, 0xFF0000FF);
	sdlwrapper_drawline(osd, audiowindow.x, middle, windowright, middle, 0, 0x000000FF);

	float scale = (float) quarterheight / (float) INT16_MAX;

	// how many samples to average into a single pixel in the window
	int samplestoaverage = 4;

	unsigned int head;
	unsigned int len;
	int16_t* buffstart = sound_getbuffer(&head, &len);
	int16_t* buffend = buffstart + len;

	// calculate an offset so that the right edge of the window matches the
	// head of the buffer
	int offset = head - (audiowindow.w * samplestoaverage) * 2;
	if (offset < 0)
		offset = len + offset;
	//log_println(LEVEL_INFO, TAG, "len %"PRId16" offset %"PRId16" buffer head %"PRId16, len, offset, head);

	int16_t* buffer = buffstart + offset;

	//g_assert(buffer < buffstart);

	for (int i = 0; i < audiowindow.w; i++) {

		int32_t left = 0;
		int32_t right = 0;

		// add up the samples
		for (int s = 0; s < samplestoaverage; s++) {
			left += *buffer++;
			right += *buffer++;
			// wrap around the buffer
			if (buffer == buffend)
				buffer = buffstart;
		}

		// average them
		left /= samplestoaverage;
		right /= samplestoaverage;

		// scale them to fit the y axis
		int16_t scaledleft = (left * scale);
		int16_t scaledright = (right * scale);

		sdlwrapper_plot(osd, audiowindow.x + i, leftbase + scaledleft, 0x00FF00FF);
		sdlwrapper_plot(osd, audiowindow.x + i, rightbase + scaledright, 0x00FF00FF);
	}
}

void osd_init() {

	osd_createlabels();

	osd = SDL_CreateRGBSurface(SDL_SWSURFACE | SDL_ASYNCBLIT, VIDEO_WIDTH, VIDEO_HEIGHT, VIDEO_PIXELFORMAT, 0, 0, 0, 0);
	colourkey = SDL_MapRGB(osd->format, 0, 0, 0);
	active = SDL_MapRGB(osd->format, 0xff, 0, 0);
	inactive = SDL_MapRGB(osd->format, 184, 184, 184);
	ledon = SDL_MapRGB(osd->format, 0, 255, 0);
	ledoff = SDL_MapRGB(osd->format, 0, 100, 0);
	audiowindowbg = SDL_MapRGB(osd->format, 0xff, 0xff, 0xff);
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

	audiowindow.h = 150;
	audiowindow.w = 200;
	audiowindow.x = 100;
	audiowindow.y = 20;

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

	osd_updateaudiobuffer();
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
