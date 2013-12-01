/*
 * osd.c
 *
 *  Created on: 16 Jun 2012
 *      Author: daniel
 */

#include <SDL_ttf.h>
#include <stdbool.h>
#include <glib.h>
#include "osd.h"
#include "renderer.h"
#include "sdlwrapper.h"
#include "fontutils.h"
#include "logging.h"
#include "hardware/board.h"
#include "hardware/cards/card.h"
#include "hardware/cards/videocard.h"
#include "hardware/cards/inputcard.h"
#include "hardware/cards/soundcard.h"
#include <SDL.h>

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

#define FREELABEL(label) (SDL_FreeSurface(label.surface))
#define BLITLABEL(label) (SDL_BlitSurface(label.surface, NULL, osd, &label.rect))
#define ORIGINLEFT(label)(label.rect.x = WINDOWPADDING)
#define ORIGINBOTTOM(label)(label.rect.y = VIDEO_HEIGHT - WINDOWPADDING - label.surface->h)
#define ORIGINTOP(label) (label.rect.y = WINDOWPADDING)
#define PUTUNDER(label, underlabel) (label.rect.y = underlabel.rect.y + underlabel.surface->h + INTERITEMPAD)
#define PUTAFTER(label, afterlabel) (label.rect.x = afterlabel.rect.x + afterlabel.surface->w + INTERITEMPAD)
#define ALIGNVERTICAL(label, alignlabel)(label.rect.y = alignlabel.rect.y);

static TTF_Font* font = NULL;
static TTF_Font* smallfont = NULL;

static SDL_Surface* osd = NULL;

static label debuglabel;
static label audiolabel;
static label dipswitcheslabel;

static label busactivitylabel;
static label statslabel, speedlabel, speedvaluelabel, overheadlabel,
		overheadvaluelabel;
static label pausekeylabel, resetkeylabel, nmikeylabel, mutekeylabel,
		osdkeylabel, throttlekeylabel, exitkeylabel;
static label audioleftlabel, audiorightlabel;

static SDL_Rect dipslabelrect, ledlabelrect, ledrect, audiowindow,
		audiowindowtitle;
static SDL_Color labels = { .r = 0, .g = 0xff, .b = 0 };
static Uint32 colourkey, ledon, ledoff, audiowindowbg;

static bool osdvisible = false;
static bool firstdraw = true;
static bool audiobufferchanged = false;

#define CLEARLABEL(label) (SDL_FillRect(osd, &label.rect, audiowindowbg))

static void osd_renderlabel(label* l, TTF_Font* f, char* t, SDL_Color* c) {
	if (l->surface != NULL)
		SDL_FreeSurface(l->surface);
	l->surface = TTF_RenderUTF8_Solid(f, t, *c);
	g_assert(l->surface != NULL);
}

static bool osd_createlabels() {
	if (TTF_Init() == 0) {
		// open the fonts
		font = TTF_OpenFont(fontutils_getmonospace(), LABELHEIGHT);
		smallfont = TTF_OpenFont(fontutils_getmonospace(), KEYLABELHEIGHT);

		if (font != NULL && smallfont != NULL) {
			// big font labels
			osd_renderlabel(&busactivitylabel, font, "bus activity", &labels);
			osd_renderlabel(&dipswitcheslabel, font, "dip switches", &labels);
			osd_renderlabel(&debuglabel, font, "debug leds", &labels);
			osd_renderlabel(&audiolabel, font, "audiobuffer", &labels);
			osd_renderlabel(&audioleftlabel, font, "left", &labels);
			osd_renderlabel(&audiorightlabel, font, "right", &labels);
			osd_renderlabel(&statslabel, font, "Statistics", &labels);
			osd_renderlabel(&speedlabel, font, "Speed: ", &labels);
			osd_renderlabel(&speedvaluelabel, font, "00.00%", &labels);
			osd_renderlabel(&overheadlabel, font, "Overhead: ", &labels);
			osd_renderlabel(&overheadvaluelabel, font, "00.00%", &labels);

			// small font labels
			osd_renderlabel(&pausekeylabel, smallfont, "[pause:F1]", &labels);
			osd_renderlabel(&resetkeylabel, smallfont, "[reset:F2]", &labels);
			osd_renderlabel(&nmikeylabel, smallfont, "[NMI:F3]", &labels);
			osd_renderlabel(&mutekeylabel, smallfont, "[mute:F4]", &labels);
			osd_renderlabel(&osdkeylabel, smallfont, "[OSD:F5]", &labels);
			osd_renderlabel(&throttlekeylabel, smallfont, "[throttle:F6]",
					&labels);
			osd_renderlabel(&exitkeylabel, smallfont, "[exit:ESC]", &labels);
			return true;
		}
	}
	return false;
}

static int quarterheight, middle, leftbase, rightbase, windowright;

static void osd_layoutaudiobuffer() {
	audiowindow.w = 200;
	audiowindow.h = 150;

	audiowindowtitle.y = WINDOWPADDING;
	audiowindowtitle.x = (VIDEO_WIDTH - WINDOWPADDING - audiowindow.w)
			+ ((audiowindow.w / 2) - (audiolabel.surface->w / 2));

	audiowindow.x = VIDEO_WIDTH - WINDOWPADDING - audiowindow.w;
	audiowindow.y = audiowindowtitle.y + audiolabel.surface->h + INTERITEMPAD;

	quarterheight = audiowindow.h / 4;
	middle = audiowindow.y + quarterheight * 2;
	leftbase = audiowindow.y + quarterheight;
	rightbase = audiowindow.y + (quarterheight * 3);

	windowright = audiowindow.x + audiowindow.w - 1;

	audioleftlabel.rect.x = audiowindow.x - audioleftlabel.surface->w
			- INTERITEMPAD;
	audioleftlabel.rect.y = leftbase - (audioleftlabel.surface->h / 2);

	audiorightlabel.rect.x = audiowindow.x - audiorightlabel.surface->w
			- INTERITEMPAD;
	audiorightlabel.rect.y = rightbase - (audiorightlabel.surface->h / 2);
}

static void osd_drawaudiobuffer() {

	if (firstdraw) {
		SDL_BlitSurface(audiolabel.surface, NULL, osd, &audiowindowtitle);
		BLITLABEL(audioleftlabel);
		BLITLABEL(audiorightlabel);
	}

	if (firstdraw || audiobufferchanged) {
		SDL_FillRect(osd, &audiowindow, audiowindowbg);
		sdlwrapper_drawline(osd, audiowindow.x, leftbase, windowright, leftbase,
				0, 0xFF0000FF);
		sdlwrapper_drawline(osd, audiowindow.x, rightbase, windowright,
				rightbase, 0, 0xFF0000FF);
		sdlwrapper_drawline(osd, audiowindow.x, middle, windowright, middle, 0,
				0xdededeFF);

		float scale = (float) quarterheight / (float) INT16_MAX;

		static int16_t left = 0;
		static int16_t right = 0;

		ringbuffer* buffer = sound_getbuffer();

		for (int i = 0; i < audiowindow.w; i++) {

			unsigned int offset = (audiowindow.w * 2) - (i * 2);

			right = ringbuffer_peek(buffer, offset);
			left = ringbuffer_peek(buffer, offset - 1);

			int16_t scaledleft = CLAMP((int16_t ) ((float ) left * scale),
					-quarterheight, quarterheight);
			int16_t scaledright = CLAMP((int16_t ) ((float ) right * scale),
					-quarterheight, quarterheight);

			sdlwrapper_plot(osd, audiowindow.x + i, leftbase + scaledleft,
					0x00FF00FF);
			sdlwrapper_plot(osd, audiowindow.x + i, rightbase + scaledright,
					0x00FF00FF);
		}
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

static void osd_layoutleds() {
	ORIGINTOP(busactivitylabel);
	ORIGINLEFT(busactivitylabel);
	dipslabelrect.x = WINDOWPADDING;
	dipslabelrect.y = busactivitylabel.rect.y + busactivitylabel.surface->h
			+ LEDHEIGHT + INTERITEMPAD;

	ledlabelrect.x = WINDOWPADDING;
	ledlabelrect.y = dipslabelrect.y + dipswitcheslabel.surface->h + LEDHEIGHT
			+ INTERITEMPAD;
	ledrect.h = LEDHEIGHT;
	ledrect.w = LEDWIDTH;
}

static void osd_drawleds() {

	static int lastbus = -1;
	static int lastleds = -1;
	uint8_t leds = inputcard_getleds();
	static bool dipschanged = true;

// draw the labels
	if (firstdraw) {
		BLITLABEL(busactivitylabel);
		SDL_BlitSurface(dipswitcheslabel.surface, NULL, osd, &dipslabelrect);
		SDL_BlitSurface(debuglabel.surface, NULL, osd, &ledlabelrect);
	}

// bus activity

	ledrect.y = busactivitylabel.rect.y + busactivitylabel.surface->h;

	int bus = 0;
	for (int i = 0; i < NUM_SLOTS; i++) {
		const card* c = board_getcardinslot(i);
		if (c != NULL && c->active != NULL && c->active())
			bus |= (1 << i);
	}

	if (firstdraw || (bus != lastbus)) {
		for (int i = 0; i < NUM_SLOTS; i++) {
			ledrect.x = busactivitylabel.rect.x
					+ ((ledrect.w + LEDSPACING) * i);
			Uint32 colour = ledoff;
			if (bus & (1 << i))
				colour = ledon;
			SDL_FillRect(osd, &ledrect, colour);
		}
		lastbus = bus;
	}
//

// dip switches
	if (firstdraw || dipschanged) {
		ledrect.x = dipslabelrect.x;
		ledrect.y = dipslabelrect.y + dipswitcheslabel.surface->h;
		for (int i = 0; i < LEDS; i++) {
			SDL_FillRect(osd, &ledrect, ledoff);
			ledrect.x += LEDSTRIDE;
		}
		dipschanged = false;
	}
//

// debug leds
	if (firstdraw || (leds != lastleds)) {
		ledrect.x = ledlabelrect.x;
		ledrect.y = ledlabelrect.y + debuglabel.surface->h;
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
	static char speedstr[10];

	if (firstdraw) {
		BLITLABEL(statslabel);
		BLITLABEL(speedlabel);
		BLITLABEL(overheadlabel);
	}

	if (speed != lastspeed) {
		snprintf(speedstr, sizeof(speedstr), "%2.2f%%", speed * 100);
		osd_renderlabel(&speedvaluelabel, font, speedstr, &labels);
		CLEARLABEL(speedvaluelabel);
		BLITLABEL(speedvaluelabel);
		lastspeed = speed;
	}
	if (overhead != lastoverhead) {
		snprintf(speedstr, sizeof(speedstr), "%2.2f%%", overhead * 100);
		osd_renderlabel(&overheadvaluelabel, font, speedstr, &labels);
		CLEARLABEL(overheadvaluelabel);
		BLITLABEL(overheadvaluelabel);
		lastoverhead = overhead;
	}
}

bool osd_init() {
	if (osd_createlabels()) {
		osd = SDL_CreateRGBSurface(0, VIDEO_WIDTH, VIDEO_HEIGHT,
		VIDEO_PIXELFORMAT, 0, 0, 0, 0);
		colourkey = SDL_MapRGB(osd->format, 0, 0, 0);
		ledon = SDL_MapRGB(osd->format, 0, 255, 0);
		ledoff = SDL_MapRGB(osd->format, 0, 100, 0);
		audiowindowbg = SDL_MapRGB(osd->format, 0xff, 0xff, 0xff);
//SDL_SetColorKey(osd, SDL_SRCCOLORKEY, colourkey);
		SDL_FillRect(osd, NULL, colourkey);

		osd_layoutleds();

		statslabel.rect.x = WINDOWPADDING;
		statslabel.rect.y = ledlabelrect.y + debuglabel.surface->h + LEDHEIGHT
				+ (INTERITEMPAD * 2);

		osd_layoutstats();

		osd_layoutkeys();
		osd_layoutaudiobuffer();
		return true;
	}
	return false;
}

void osd_update(double speed, double overhead) {
	if (!osdvisible)
		return;
	osd_drawleds();
	osd_drawaudiobuffer();
	osd_drawkeys();
	osd_drawstats(speed, overhead);
	firstdraw = false;
	renderer_requestrender();
}

void osd_toggle() {
	osd_visible(!osdvisible);
}

void osd_visible(bool visible) {
	if (!osdvisible && visible)
		firstdraw = true;

	osdvisible = visible;
	renderer_requestrender();
}

void osd_render(SDL_Surface* screen) {
	if (osdvisible) {
		//SDL_Surface* temp = SDL_DisplayFormat(osd);
		//SDL_BlitSurface(temp, NULL, screen, NULL);
		//SDL_FreeSurface(temp);
	}
}

void osd_dispose() {
	FREELABEL(busactivitylabel);
	SDL_FreeSurface(debuglabel.surface);
	SDL_FreeSurface(dipswitcheslabel.surface);

	FREELABEL(audioleftlabel);
	FREELABEL(audiorightlabel);

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
