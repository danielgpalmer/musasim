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
#define KEYLABELHEIGHT 10

#define WINDOWPADDING 10
#define INTERITEMPAD 5

#define LEDHEIGHT  10
#define LEDWIDTH  15
#define LEDSPACING  5
#define LEDSTRIDE (LEDWIDTH + LEDSPACING)
#define LEDS 8

typedef struct {
	SDL_Surface* surface;
	SDL_Rect rect;
} label;

#define RENDERLABEL(label, font, text, colour) (label.surface = TTF_RenderUTF8_Solid(font, text, colour))
#define FREELABEL(label) (SDL_FreeSurface(label.surface))
#define BLITLABEL(label) (SDL_BlitSurface(label.surface, NULL, osd, &label.rect))
#define ORIGINLEFT(label)(label.rect.x = WINDOWPADDING)
#define ORIGINBOTTOM(label)(label.rect.y = VIDEO_HEIGHT - WINDOWPADDING - label.surface->h)
#define PUTUNDER(label, underlabel) (label.rect.y = underlabel.rect.y + underlabel.surface->h + INTERITEMPAD)
#define PUTAFTER(label, afterlabel) (label.rect.x = afterlabel.rect.x + afterlabel.surface->w + INTERITEMPAD)
#define ALIGNVERTICAL(label, alignlabel)(label.rect.y = alignlabel.rect.y);

static TTF_Font* font = NULL;
static TTF_Font* smallfont = NULL;

static SDL_Surface* osd = NULL;
static SDL_Surface* busactivitylabel;
static SDL_Surface* dipswitcheslabel;
static SDL_Surface* debuglabel;
static SDL_Surface* audiolabel;
static SDL_Surface* leftlabel;
static SDL_Surface* rightlabel;

static label statslabel, speedlabel, speedvaluelabel, overheadlabel, overheadvaluelabel;
static label pausekeylabel, resetkeylabel, nmikeylabel, mutekeylabel, osdkeylabel, throttlekeylabel, exitkeylabel;

static SDL_Rect busactivitylabelrect, dipslabelrect, ledlabelrect, ledrect, audiowindow, audiowindowtitle,
		audiowindowlabels;
static SDL_Color labels = { .r = 0, .g = 0xff, .b = 0 };
static Uint32 colourkey, ledon, ledoff, audiowindowbg;
static bool osdvisible = false;
static bool firstdraw = true;

#define CLEARLABEL(label) (SDL_FillRect(osd, &label.rect, audiowindowbg))

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
		audiolabel = TTF_RenderUTF8_Solid(font, "audiobuffer", labels);
		leftlabel = TTF_RenderUTF8_Solid(font, "left", labels);
		rightlabel = TTF_RenderUTF8_Solid(font, "right", labels);
		RENDERLABEL(statslabel, font, "Statistics", labels);
		RENDERLABEL(speedlabel, font, "Speed: ", labels);
		RENDERLABEL(speedvaluelabel, font, "00.00%", labels);
		RENDERLABEL(overheadlabel, font, "Overhead: ", labels);
		RENDERLABEL(overheadvaluelabel, font, "00.00%", labels);
	}
	smallfont = TTF_OpenFont(fontutils_getmonospace(), KEYLABELHEIGHT);
	if (smallfont != NULL ) {
		RENDERLABEL(pausekeylabel, smallfont, "[pause:F1]", labels);
		RENDERLABEL(resetkeylabel, smallfont, "[reset:F2]", labels);
		RENDERLABEL(nmikeylabel, smallfont, "[NMI:F3]", labels);
		RENDERLABEL(mutekeylabel, smallfont, "[mute:F4]", labels);
		RENDERLABEL(osdkeylabel, smallfont, "[OSD:F5]", labels);
		RENDERLABEL(throttlekeylabel, smallfont, "[throttle:F6]", labels);
		RENDERLABEL(exitkeylabel, smallfont, "[exit:ESC]", labels);
	}

}

static void osd_updateaudiobuffer() {
	int quarterheight = audiowindow.h / 4;
	int middle = audiowindow.y + quarterheight * 2;
	int leftbase = audiowindow.y + quarterheight;
	int rightbase = audiowindow.y + (quarterheight * 3);

	int windowright = audiowindow.x + audiowindow.w - 1;

	SDL_FillRect(osd, &audiowindow, audiowindowbg);
	sdlwrapper_drawline(osd, audiowindow.x, leftbase, windowright, leftbase, 0, 0xFF0000FF);
	sdlwrapper_drawline(osd, audiowindow.x, rightbase, windowright, rightbase, 0, 0xFF0000FF);
	sdlwrapper_drawline(osd, audiowindow.x, middle, windowright, middle, 0, 0xdededeFF);

	float scale = (float) quarterheight / (float) INT16_MAX;

	static int16_t left = 0;
	static int16_t right = 0;

	SDL_BlitSurface(audiolabel, NULL, osd, &audiowindowtitle);

	audiowindowlabels.x = audiowindow.x - leftlabel->w - INTERITEMPAD;
	audiowindowlabels.y = leftbase - (leftlabel->h / 2);
	SDL_BlitSurface(leftlabel, NULL, osd, &audiowindowlabels);
	audiowindowlabels.x = audiowindow.x - rightlabel->w - INTERITEMPAD;
	audiowindowlabels.y = rightbase - (rightlabel->h / 2);
	SDL_BlitSurface(rightlabel, NULL, osd, &audiowindowlabels);

	ringbuffer* buffer = sound_getbuffer();

	for (int i = 0; i < audiowindow.w; i++) {

		unsigned int offset = (audiowindow.w * 2) - (i * 2);

		right = ringbuffer_peek(buffer, offset);
		left = ringbuffer_peek(buffer, offset - 1);

		int16_t scaledleft = CLAMP((int16_t) ((float) left * scale), -quarterheight, quarterheight);
		int16_t scaledright = CLAMP((int16_t) ((float) right * scale), -quarterheight, quarterheight);

		sdlwrapper_plot(osd, audiowindow.x + i, leftbase + scaledleft, 0x00FF00FF);
		sdlwrapper_plot(osd, audiowindow.x + i, rightbase + scaledright, 0x00FF00FF);
	}
}

static void osd_layoutkeys() {
	ORIGINLEFT(pausekeylabel);
	ORIGINBOTTOM(pausekeylabel);

	ALIGNVERTICAL(resetkeylabel, pausekeylabel);
	ALIGNVERTICAL(nmikeylabel, pausekeylabel);
	ALIGNVERTICAL(mutekeylabel, pausekeylabel);
	ALIGNVERTICAL(osdkeylabel, pausekeylabel);
	ALIGNVERTICAL(throttlekeylabel, pausekeylabel);
	ALIGNVERTICAL(exitkeylabel, pausekeylabel);

	PUTAFTER(resetkeylabel, pausekeylabel);
	PUTAFTER(nmikeylabel, resetkeylabel);
	PUTAFTER(mutekeylabel, nmikeylabel);
	PUTAFTER(osdkeylabel, mutekeylabel);
	PUTAFTER(throttlekeylabel, osdkeylabel);
	PUTAFTER(exitkeylabel, throttlekeylabel);
}

static void osd_drawkeys() {
	if (firstdraw) {
		BLITLABEL(pausekeylabel);
		BLITLABEL(resetkeylabel);
		BLITLABEL(nmikeylabel);
		BLITLABEL(mutekeylabel);
		BLITLABEL(osdkeylabel);
		BLITLABEL(throttlekeylabel);
		BLITLABEL(exitkeylabel);
	}
}

static void osd_drawleds() {

	static int lastleds = -1;
	uint8_t leds = inputcard_getleds();
	static bool dipschanged = true;

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
	if (dipschanged) {
		SDL_BlitSurface(dipswitcheslabel, NULL, osd, &dipslabelrect);
		ledrect.x = dipslabelrect.x;
		ledrect.y = dipslabelrect.y + dipswitcheslabel->h;
		for (int i = 0; i < LEDS; i++) {
			SDL_FillRect(osd, &ledrect, ledoff);
			ledrect.x += LEDSTRIDE;
		}
		dipschanged = false;
	}
//

// debug leds
	if (leds != lastleds) {
		SDL_BlitSurface(debuglabel, NULL, osd, &ledlabelrect);
		ledrect.x = ledlabelrect.x;
		ledrect.y = ledlabelrect.y + debuglabel->h;
		for (int i = 0; i < LEDS; i++) {
			SDL_FillRect(osd, &ledrect, ((leds >> i) & 1) ? ledon : ledoff);
			ledrect.x += LEDSTRIDE;
		}
		lastleds = leds;
	}
//
}

static void osd_layoutstats() {
	ORIGINLEFT(speedlabel);
	PUTUNDER(speedlabel, statslabel);

	ALIGNVERTICAL(speedvaluelabel, speedlabel);
	PUTAFTER(speedvaluelabel, speedlabel);

	ORIGINLEFT(overheadlabel);
	PUTUNDER(overheadlabel, speedlabel);

	ALIGNVERTICAL(overheadvaluelabel, overheadlabel);
	PUTAFTER(overheadvaluelabel, overheadlabel);
}

static void osd_drawstats(double speed, double overhead) {
	static double lastspeed = -1;
	static double lastoverhead = -1;

	if (firstdraw) {
		BLITLABEL(statslabel);
		BLITLABEL(speedlabel);
		BLITLABEL(overheadlabel);
	}

	if (speed != lastspeed) {
		static char speedstr[10];
		snprintf(speedstr, sizeof(speedstr), "%2.2f%%", speed * 100);
		SDL_FreeSurface(speedvaluelabel.surface);
		speedvaluelabel.surface = TTF_RenderUTF8_Solid(font, speedstr, labels);
		CLEARLABEL(speedvaluelabel);
		BLITLABEL(speedvaluelabel);
		lastspeed = speed;
	}
	if (overhead != lastoverhead) {
		static char speedstr[10];
		snprintf(speedstr, sizeof(speedstr), "%2.2f%%", overhead * 100);
		SDL_FreeSurface(overheadvaluelabel.surface);
		overheadvaluelabel.surface = TTF_RenderUTF8_Solid(font, speedstr, labels);
		CLEARLABEL(overheadvaluelabel);
		BLITLABEL(overheadvaluelabel);
		lastoverhead = overhead;
	}
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

	busactivitylabelrect.x = WINDOWPADDING;
	busactivitylabelrect.y = WINDOWPADDING;

	dipslabelrect.x = WINDOWPADDING;
	dipslabelrect.y = busactivitylabelrect.y + busactivitylabel->h + LEDHEIGHT + INTERITEMPAD;

	ledlabelrect.x = WINDOWPADDING;
	ledlabelrect.y = dipslabelrect.y + dipswitcheslabel->h + LEDHEIGHT + INTERITEMPAD;

	statslabel.rect.x = WINDOWPADDING;
	statslabel.rect.y = ledlabelrect.y + debuglabel->h + LEDHEIGHT + (INTERITEMPAD * 2);

	osd_layoutstats();

	ledrect.h = LEDHEIGHT;
	ledrect.w = LEDWIDTH;

	audiowindow.w = 200;
	audiowindow.h = 150;

	audiowindowtitle.y = WINDOWPADDING;
	audiowindowtitle.x = (VIDEO_WIDTH - WINDOWPADDING - audiowindow.w) + ((audiowindow.w / 2) - (audiolabel->w / 2));

	audiowindow.x = VIDEO_WIDTH - WINDOWPADDING - audiowindow.w;
	audiowindow.y = audiowindowtitle.y + audiolabel->h + INTERITEMPAD;

	osd_layoutkeys();

	osd_set();
}

void osd_update(double speed, double overhead) {
	if (!osdvisible)
		return;
	osd_drawleds();
	osd_updateaudiobuffer();
	osd_drawkeys();
	osd_drawstats(speed, overhead);
	firstdraw = false;
}

void osd_toggle() {
	osd_visible(!osdvisible);
}

void osd_visible(bool visible) {
	if (!osdvisible && visible)
		firstdraw = true;

	osdvisible = visible;
	osd_set();
}

void osd_dispose() {
	SDL_FreeSurface(debuglabel);
	SDL_FreeSurface(busactivitylabel);
	SDL_FreeSurface(dipswitcheslabel);

	FREELABEL(pausekeylabel);
	FREELABEL(resetkeylabel);
	FREELABEL(nmikeylabel);
	FREELABEL(mutekeylabel);
	FREELABEL(osdkeylabel);
	FREELABEL(throttlekeylabel);
	FREELABEL(exitkeylabel);

	SDL_FreeSurface(osd);
	TTF_CloseFont(font);
	TTF_CloseFont(smallfont);
	TTF_Quit();
}
