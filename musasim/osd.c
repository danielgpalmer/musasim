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

#define LABELHEIGHT 15
#define KEYLABELHEIGHT 12

#define WINDOWPADDING 10
#define INTERITEMPAD 5

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

static SDL_Surface* pausekeylabel;
static SDL_Surface* resetkeylabel;
static SDL_Surface* nmikeylabel;
static SDL_Surface* mutekeylabel;
static SDL_Surface* osdkeylabel;
static SDL_Surface* throttlekeylabel;
static SDL_Surface* exitkeylabel;

static SDL_Rect busactivitylabelrect, dipslabelrect, ledlabelrect, ledrect, audiowindow;
static SDL_Color labels = { .r = 0, .g = 0xff, .b = 0 };
static Uint32 colourkey, ledon, ledoff, audiowindowbg;
static bool osdvisible = false;

static void osd_set() {
	videocard_setosd(osdvisible ? osd : NULL );
}

void osd_createlabels() {
	TTF_Init();
	font = TTF_OpenFont(fontutils_getmonospace(), LABELHEIGHT);
	if (font != NULL ) {
		busactivitylabel = TTF_RenderUTF8_Solid(font, "bus activity", labels);
		dipswitcheslabel = TTF_RenderUTF8_Solid(font, "dip switches", labels);
		debuglabel = TTF_RenderUTF8_Solid(font, "debug leds", labels);
		TTF_CloseFont(font);
	}
	font = TTF_OpenFont(fontutils_getmonospace(), KEYLABELHEIGHT);
	if (font != NULL ) {
		pausekeylabel = TTF_RenderUTF8_Solid(font, "[pause:F1]", labels);
		resetkeylabel = TTF_RenderUTF8_Solid(font, "[reset:F2]", labels);
		nmikeylabel = TTF_RenderUTF8_Solid(font, "[NMI:F3]", labels);
		mutekeylabel = TTF_RenderUTF8_Solid(font, "[mute:F4]", labels);
		osdkeylabel = TTF_RenderUTF8_Solid(font, "[OSD:F5]", labels);
		throttlekeylabel = TTF_RenderUTF8_Solid(font, "[throttle:F6]", labels);
		exitkeylabel = TTF_RenderUTF8_Solid(font, "[exit:ESC]", labels);
		TTF_CloseFont(font);
	}

	TTF_Quit();
}

static void osd_updateaudiobuffer() {

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
	//int samplestoaverage = 4;

	//ringbuffer* buffstart = sound_getbuffer();

	// calculate an offset so that the right edge of the window matches the
	// head of the buffer
	//int offset = head - (audiowindow.w * samplestoaverage) * 2;
	//if (offset < 0)
	//	offset = len + offset;
	//log_println(LEVEL_INFO, TAG, "len %"PRId16" offset %"PRId16" buffer head %"PRId16, len, offset, head);

	//int16_t* buffer = buffstart + offset;

	//g_assert(buffer < buffstart);

	for (int i = 0; i < audiowindow.w; i++) {

		int32_t left = 0;
		int32_t right = 0;

		// add up the samples
		//for (int s = 0; s < samplestoaverage; s++) {
		//	left += *buffer++;
		//	right += *buffer++;
		//	// wrap around the buffer
		//	if (buffer == buffend)
		//		buffer = buffstart;
		//}

		// average them
		//left /= samplestoaverage;
		//right /= samplestoaverage;

		// scale them to fit the y axis
		int16_t scaledleft = (left * scale);
		int16_t scaledright = (right * scale);

		sdlwrapper_plot(osd, audiowindow.x + i, leftbase + scaledleft, 0x00FF00FF);
		sdlwrapper_plot(osd, audiowindow.x + i, rightbase + scaledright, 0x00FF00FF);
	}
}

static void osd_drawkeys() {
	static SDL_Rect rect;
	rect.y = VIDEO_HEIGHT - WINDOWPADDING - pausekeylabel->h;
	rect.x = WINDOWPADDING;

	SDL_BlitSurface(pausekeylabel, NULL, osd, &rect);
	rect.x += pausekeylabel->w;
	SDL_BlitSurface(resetkeylabel, NULL, osd, &rect);
	rect.x += resetkeylabel->w;
	SDL_BlitSurface(nmikeylabel, NULL, osd, &rect);
	rect.x += nmikeylabel->w;
	SDL_BlitSurface(mutekeylabel, NULL, osd, &rect);
	rect.x += mutekeylabel->w;
	SDL_BlitSurface(osdkeylabel, NULL, osd, &rect);
	rect.x += osdkeylabel->w;
	SDL_BlitSurface(throttlekeylabel, NULL, osd, &rect);
	rect.x += throttlekeylabel->w;
	SDL_BlitSurface(exitkeylabel, NULL, osd, &rect);
}

static void osd_drawleds() {
	// bus activity
	SDL_BlitSurface(busactivitylabel, NULL, osd, &busactivitylabelrect);
	ledrect.y = busactivitylabelrect.y + busactivitylabel->h;

	for (int i = 0; i < NUM_SLOTS; i++) {
		ledrect.x = busactivitylabelrect.x + ((ledrect.w + LEDSPACING) * i);

		Uint32 colour = ledoff;
		const card* c = board_getcardinslot(i);

		if (c != NULL && c->active != NULL ) {
			if (c->active()) {
				colour = ledon;
			}
		}

		SDL_FillRect(osd, &ledrect, colour);
	}
	//

	// dip switches
	SDL_BlitSurface(dipswitcheslabel, NULL, osd, &dipslabelrect);
	ledrect.x = dipslabelrect.x;
	ledrect.y = dipslabelrect.y + dipswitcheslabel->h;
	for (int i = 0; i < LEDS; i++) {
		SDL_FillRect(osd, &ledrect, ledoff);
		ledrect.x += LEDSTRIDE;
	}
	//

	// debug leds
	SDL_BlitSurface(debuglabel, NULL, osd, &ledlabelrect);
	uint8_t leds = inputcard_getleds();
	ledrect.x = ledlabelrect.x;
	ledrect.y = ledlabelrect.y + debuglabel->h;
	for (int i = 0; i < LEDS; i++) {
		SDL_FillRect(osd, &ledrect, ((leds >> i) & 1) ? ledon : ledoff);
		ledrect.x += LEDSTRIDE;
	}
	//
}

void osd_init() {
	osd_createlabels();

	osd = SDL_CreateRGBSurface(SDL_SWSURFACE | SDL_ASYNCBLIT, VIDEO_WIDTH, VIDEO_HEIGHT, VIDEO_PIXELFORMAT, 0, 0, 0, 0);
	colourkey = SDL_MapRGB(osd->format, 0, 0, 0);
	ledon = SDL_MapRGB(osd->format, 0, 255, 0);
	ledoff = SDL_MapRGB(osd->format, 0, 100, 0);
	audiowindowbg = SDL_MapRGB(osd->format, 0xff, 0xff, 0xff);
	SDL_SetColorKey(osd, SDL_SRCCOLORKEY, colourkey);
	SDL_FillRect(osd, NULL, colourkey);

	SDL_FillRect(osd, &audiowindow, audiowindowbg);

	busactivitylabelrect.x = WINDOWPADDING;
	busactivitylabelrect.y = WINDOWPADDING;

	dipslabelrect.x = WINDOWPADDING;
	dipslabelrect.y = busactivitylabelrect.y + busactivitylabel->h + LEDHEIGHT + INTERITEMPAD;

	ledlabelrect.x = WINDOWPADDING;
	ledlabelrect.y = dipslabelrect.y + dipswitcheslabel->h + LEDHEIGHT + INTERITEMPAD;

	ledrect.h = LEDHEIGHT;
	ledrect.w = LEDWIDTH;

	audiowindow.h = 150;
	audiowindow.w = 200;
	audiowindow.x = VIDEO_WIDTH - WINDOWPADDING - audiowindow.w;
	audiowindow.y = WINDOWPADDING;

	osd_set();
}

void osd_update() {
	if (!osdvisible)
		return;
	osd_drawleds();
	osd_updateaudiobuffer();
	osd_drawkeys();
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
	SDL_FreeSurface(pausekeylabel);
	SDL_FreeSurface(resetkeylabel);
	SDL_FreeSurface(nmikeylabel);
	SDL_FreeSurface(mutekeylabel);
	SDL_FreeSurface(osdkeylabel);
	SDL_FreeSurface(throttlekeylabel);
	SDL_FreeSurface(exitkeylabel);
	SDL_FreeSurface(osd);
}
