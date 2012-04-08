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
static SDL_Surface* rendersurface = NULL;
static SDL_Rect region;
static SDL_Rect window;

static uint32_t registersstart;

#define GETVIDREG(x) ( x = 0 ? 0 : (x & ~registersstart) / 2)

static void video_init() {

	log_println(LEVEL_DEBUG, TAG, "video_init()");

	screen = SDL_SetVideoMode(VIDEO_WIDTH, VIDEO_HEIGHT, VIDEO_PIXELFORMAT, SDL_SWSURFACE);
	rendersurface = SDL_CreateRGBSurface(SDL_SWSURFACE, VIDEO_PLAYFIELDWIDTH, VIDEO_PLAYFIELDHEIGHT, VIDEO_PIXELFORMAT,
			0, 0, 0, 0);

	SDL_FillRect(rendersurface, NULL, 0xFF0000FF);

	log_println(LEVEL_INFO, TAG, "Created surface; %d x %d pixels @ %dBPP", screen->w, screen->h,
			screen->format->BitsPerPixel);

	registersstart = utils_nextpow(VIDEO_MEMORYEND);
	log_println(LEVEL_DEBUG, TAG, "Memory size is 0x%x, registers start at 0x%x", VIDEO_MEMORYEND, registersstart);
	log_println(LEVEL_DEBUG, TAG,
			"Active area is %d pixel, Total area  is %d pixels, refresh rate %d, pixels per second %d, pixels per tick %d",
			VIDEO_ACTIVEPIXELS, VIDEO_TOTALPIXELS, VIDEO_REFRESHRATE, VIDEO_PIXELSPERSECOND, PIXELSPERTICK);

}

static void video_dispose() {

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
					SDL_BlitSurface(rendersurface, &region, screen, &window);
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

	if (!video_validaddress(address)) {
		return;
	}

	if (address < registersstart) {
		if (SDL_MUSTLOCK(rendersurface)) {
			SDL_LockSurface(rendersurface);
		}
		*((uint8_t*) rendersurface->pixels + address) = data;
		if (SDL_MUSTLOCK(rendersurface)) {
			SDL_UnlockSurface(rendersurface);
		}
	}
}

static void video_write_word(uint32_t address, uint16_t data) {

	if (!video_validaddress(address)) {
		return;
	}

	if (address < registersstart) {
		if (SDL_MUSTLOCK(screen)) {
			SDL_LockSurface(rendersurface);
		}
		*((uint16_t*) rendersurface->pixels + (address / 2)) = data;
		if (SDL_MUSTLOCK(rendersurface)) {
			SDL_UnlockSurface(screen);
		}
	}

	else {
		uint8_t reg = GETVIDREG(address);
		*video_registers[reg] = data;
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
		log_println(LEVEL_INSANE, TAG, "Read video register %d", reg);
		return *(video_registers[reg]);
	}

	return 0;

}

static void videocard_irqack() {
	board_lower_interrupt(&videocard);
}

const card videocard = { "VIDEO CARD", video_init, video_dispose, video_tick, videocard_irqack, NULL, video_read_byte,
		video_read_word, NULL, video_write_byte, video_write_word, NULL };
