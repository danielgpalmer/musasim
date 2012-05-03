#include <stdint.h>
#include <stdbool.h>
#include <unistd.h>
#include <SDL/SDL.h>
#include <video_registermasks.h>

#include "../board.h"
#include "videocard.h"
#include "../../sim.h"
#include "../../logging.h"
#include "../../utils.h"

#define PIXELSPERTICK (VIDEO_PIXELSPERSECOND/SIM_TICKS_PERSECOND)

static const char TAG[] = "video";

static uint16_t flags = 0;
static uint16_t config = 0;
static uint16_t pixel = 0;
static uint16_t line = 0;
static uint16_t frame = 0;
static uint16_t posx = 0;
static uint16_t posy = 0;
static uint16_t winx = 0;
static uint16_t winy = 0;
static uint16_t winwidth = VIDEO_WIDTH;
static uint16_t winheight = VIDEO_HEIGHT;

static uint16_t* video_registers[] = { &flags, &config, &pixel, &line, &frame, &posx, &posy, &winx, &winy, &winwidth,
		&winheight };

static SDL_Surface* screen = NULL;
static SDL_Surface* rendersurfaces[2];
static SDL_Rect region;
static SDL_Rect window;

static uint32_t registersstart;

#define GETVIDREG(x) ( x = 0 ? 0 : (x & ~registersstart) / 2)

#define WRITEABLESURFACE ((config & VIDEO_CONFIG_FLIP) ? rendersurfaces[0] : rendersurfaces[1])
#define VISIBLESURFACE ( !(config & VIDEO_CONFIG_FLIP) ? rendersurfaces[1] : rendersurfaces[0])

static void video_init() {

	log_println(LEVEL_DEBUG, TAG, "video_init()");

	screen = SDL_SetVideoMode(VIDEO_WIDTH, VIDEO_HEIGHT, VIDEO_PIXELFORMAT, SDL_SWSURFACE);

	for (int i = 0; i < SIZEOFARRAY(rendersurfaces); i++) {

		SDL_Surface* rendersurface = NULL;

		rendersurface = SDL_CreateRGBSurface(SDL_SWSURFACE, VIDEO_PLAYFIELDWIDTH, VIDEO_PLAYFIELDHEIGHT,
				VIDEO_PIXELFORMAT, 0, 0, 0, 0);

		if (rendersurface != NULL) {
			SDL_FillRect(rendersurface, NULL, 0);
			// FIXME exit here
		}
		rendersurfaces[i] = rendersurface;

	}

	log_println(LEVEL_INFO, TAG, "Created surface; %d x %d pixels @ %dBPP", screen->w, screen->h,
			screen->format->BitsPerPixel);

	registersstart = utils_nextpow(VIDEO_MEMORYEND);
	log_println(LEVEL_DEBUG, TAG, "Memory size is 0x%x, registers start at 0x%x", VIDEO_MEMORYEND, registersstart);
	log_println(LEVEL_DEBUG, TAG,
			"Active area is %d pixel, Total area  is %d pixels, refresh rate %d, pixels per second %d, pixels per tick %d",
			VIDEO_ACTIVEPIXELS, VIDEO_TOTALPIXELS, VIDEO_REFRESHRATE, VIDEO_PIXELSPERSECOND, PIXELSPERTICK);

}

static void video_dispose() {

	for (int i = 0; i < SIZEOFARRAY(rendersurfaces); i++) {
		SDL_FreeSurface(rendersurfaces[i]);
	}

	log_println(LEVEL_DEBUG, TAG, "video_dispose()");
}

static bool video_validaddress(uint32_t address) {

	if (address < VIDEO_MEMORYEND) {
		if ((config & VIDEO_CONFIG_MODE_MASK) == VIDEO_CONFIG_MODE_DISABLED) {
			log_println(LEVEL_DEBUG, TAG, "access to video memory while disabled");
			return false;
		}
		return true;
	}

	else if (address >= registersstart && address < registersstart + sizeof(video_registers)) {
		return true;
	}

	printf("0x%x is not a valid address\n", address);

	return false;
}

static void video_tick() {

	int mode = config & VIDEO_CONFIG_MODE_MASK;

	if (mode == VIDEO_CONFIG_MODE_DISABLED) {
		return;
	}

	log_println(LEVEL_INSANE, TAG, "video_tick()");

	//int ticksperpixel = VIDEO_TOTALPIXELS;

	//printf("ticks per pixel %d %d\n", ticksperpixel, SIM_TICKS_PERSECOND);

	for (int i = 0; i < PIXELSPERTICK; i++) {

		pixel++;

		if (pixel == VIDEO_WIDTH) {
			flags |= FLAG_HBLANK;
			if (config & VIDEO_CONFIG_ENHBINT) {
				board_raise_interrupt(&videocard);
			}
		}

		else if (pixel == (VIDEO_WIDTH + HBLANKPERIOD)) {
			flags &= !FLAG_HBLANK;
			pixel = 0;
			line++;

			if (line == VIDEO_HEIGHT) {
				flags |= FLAG_VBLANK;
				if (config & VIDEO_CONFIG_ENVBINT) {

					region.x = posx + winx;
					region.y = posy + winy;
					region.w = winwidth;
					region.h = winheight;

					window.x = winx;
					window.y = winy;
					window.w = 0;
					window.h = 0;

					SDL_FillRect(screen, NULL, 0x0);
					SDL_BlitSurface(VISIBLESURFACE, &region, screen, &window);
					if (config & VIDEO_CONFIG_ENVBINT) {
						board_raise_interrupt(&videocard);
					}
				}

			}

			else if (line == (VIDEO_HEIGHT + VBLANKPERIOD)) {
				flags &= ~FLAG_VBLANK;
				line = 0;
				SDL_Flip(screen);
				frame++;
			}
		}
	}

	//printf("%d, %d\n", pixel, line);
}

static void video_write_byte(uint32_t address, uint8_t data) {

	if (address < registersstart) {
		if (SDL_MUSTLOCK(WRITEABLESURFACE)) {
			SDL_LockSurface(WRITEABLESURFACE);
		}
		*((uint8_t*) WRITEABLESURFACE->pixels + address) = data;
		if (SDL_MUSTLOCK(WRITEABLESURFACE)) {
			SDL_UnlockSurface(WRITEABLESURFACE);
		}
	}
}

static void video_write_word(uint32_t address, uint16_t data) {

	if (address < registersstart) {
		if (SDL_MUSTLOCK(screen)) {
			SDL_LockSurface(WRITEABLESURFACE);
		}
		*((uint16_t*) WRITEABLESURFACE->pixels + (address / 2)) = data;
		if (SDL_MUSTLOCK(WRITEABLESURFACE)) {
			SDL_UnlockSurface(screen);
		}
	}

	else {
		uint8_t reg = GETVIDREG(address);
		if (reg == 1) {
			if (*(video_registers[reg]) & VIDEO_CONFIG_FLIP) {
				log_println(LEVEL_DEBUG, TAG, "surface 1");
			}
			else {
				log_println(LEVEL_DEBUG, TAG, "surface 0");
			}
		}
		*(video_registers[reg]) = data;
	}

}

static uint8_t video_read_byte(uint32_t address) {

	if (!video_validaddress(address)) {
		return 0;
	}

	return 0;

}

static uint16_t video_read_word(uint32_t address) {

	if (!video_validaddress(address)) {
		return 0;
	}

	if (address >= registersstart) {
		uint8_t reg = GETVIDREG(address);
		return *(video_registers[reg]);
	}

	return 0;

}

static void videocard_irqack() {
	board_lower_interrupt(&videocard);
}

const card videocard = { "VIDEO CARD", video_init, video_dispose, NULL, video_tick, videocard_irqack, NULL,
		video_validaddress, video_read_byte, video_read_word, NULL, video_write_byte, video_write_word, NULL };
