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

static const char TAG[] = "video";

static bool vramtouched = false;

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
static SDL_Surface* osd = NULL;
static SDL_Rect region;
static SDL_Rect window;
static void* compositingbuffer;

#define GETVIDREG(x) ( x = 0 ? 0 : (x / 2)) //FIXME there should be mirrors of the registers
#define FRONTSURFACE	((config & VIDEO_CONFIG_FLIP) ? rendersurfaces[0] : rendersurfaces[1])
#define BACKSURFACE 		((config & VIDEO_CONFIG_FLIP) ? rendersurfaces[1] : rendersurfaces[0])

#define ISACTIVE (!(flags & FLAG_HBLANK || flags & FLAG_VBLANK))

#define HBLANKENABLED (config & VIDEO_CONFIG_ENHBINT)
#define VBLANKENABLED (config & VIDEO_CONFIG_ENVBINT)

static void video_updaterects() {
	region.x = posx + winx;
	region.y = posy + winy;
	region.w = winwidth;
	region.h = winheight;

	window.x = winx;
	window.y = winy;
	window.w = 0;
	window.h = 0;
}

static void video_init() {

	log_println(LEVEL_DEBUG, TAG, "video_init()");

	screen = SDL_SetVideoMode(VIDEO_WIDTH, VIDEO_HEIGHT, VIDEO_PIXELFORMAT, SDL_SWSURFACE);

	for (int i = 0; i < SIZEOFARRAY(rendersurfaces); i++) {

		SDL_Surface* rendersurface = NULL;

		rendersurface = SDL_CreateRGBSurface(SDL_SWSURFACE | SDL_ASYNCBLIT, VIDEO_PLAYFIELDWIDTH, VIDEO_PLAYFIELDHEIGHT,
				VIDEO_PIXELFORMAT, 0, 0, 0, 0);

		if (rendersurface != NULL) {
			SDL_FillRect(rendersurface, NULL, 0);
			// FIXME exit here
		}
		rendersurfaces[i] = rendersurface;

	}

	compositingbuffer = malloc(VIDEO_COMPOSITINGBUFFER_SIZE);

	log_println(LEVEL_INFO, TAG, "Created surface; %d x %d pixels @ %dBPP", screen->w, screen->h,
			screen->format->BitsPerPixel);

	log_println(LEVEL_DEBUG, TAG, "Active area is %d pixel, Total area  is %d pixels, refresh rate %d, pixelclock %d",
			VIDEO_ACTIVEPIXELS, VIDEO_TOTALPIXELS, VIDEO_REFRESHRATE, VIDEO_PIXELCLOCK);

	video_updaterects();

}

static void video_dispose() {

	for (int i = 0; i < SIZEOFARRAY(rendersurfaces); i++) {
		SDL_FreeSurface(rendersurfaces[i]);
	}

	free(compositingbuffer);

	log_println(LEVEL_DEBUG, TAG, "video_dispose()");
}

static bool video_validaddress(uint32_t address) {

	//if (address >= registersstart && address < registersstart + sizeof(video_registers)) {
	//	return true;
	//}
	return true;
}

static void video_tick(int cyclesexecuted) {

	//int mode = config & VIDEO_CONFIG_MODE_MASK;

	//if (mode == VIDEO_CONFIG_MODE_DISABLED) {
	//	return;
	//}

	int pixelclocks = cyclesexecuted / VIDEO_MACHINECLOCKSPERPIXELCLOCK;
	//log_println(LEVEL_INFO, TAG, "machine clocks %d, video clocks %d", cyclesexecuted, pixelclocks);

	for (int i = 0; i < pixelclocks; i++) {

		if (pixel == 0 && line == 0) {
			log_println(LEVEL_INFO, TAG, "refresh");
			videocard_refresh();
		}

		// hblank handling
		if (pixel == (VIDEO_WIDTH - 1)) {
			flags |= FLAG_HBLANK;
			if (HBLANKENABLED) {
				board_raise_interrupt(&videocard);
			}
		}

		// line handling
		else if (pixel == (VIDEO_WIDTH + VIDEO_HBLANKPERIOD - 1)) {
			// turn hblank off
			flags &= !FLAG_HBLANK;
			// vblank handling
			if (line == VIDEO_HEIGHT - 1) {
				flags |= FLAG_VBLANK;
				//log_println(LEVEL_INFO, TAG, "v blank start");
				if (VBLANKENABLED) {
					board_raise_interrupt(&videocard);
				}

			} // FIXME
			else if (line == (VIDEO_HEIGHT + VIDEO_VBLANKPERIOD - 1)) {
				// turn vblank off
				flags &= ~FLAG_VBLANK;
				//log_println(LEVEL_INFO, TAG, "v blank end");
			}

			line++;
			if (line == VIDEO_HEIGHT + VIDEO_VBLANKPERIOD) {
				line = 0;
			}

		}

		pixel++;
		if (pixel == VIDEO_WIDTH + VIDEO_HBLANKPERIOD) {
			pixel = 0;
		}

	}
}

static void video_write_byte(uint32_t address, uint8_t data) {

	if (address < VIDEO_FRAMEBUFFER_START) {

	}

	else if (address < VIDEO_COMPOSITINGBUFFER_START) {
		if (SDL_MUSTLOCK(FRONTSURFACE)) {
			SDL_LockSurface(FRONTSURFACE);
		}
		*((uint8_t*) FRONTSURFACE->pixels + (address - VIDEO_FRAMEBUFFER_START)) = data;
		vramtouched = true;
		if (SDL_MUSTLOCK(FRONTSURFACE)) {
			SDL_UnlockSurface(FRONTSURFACE);
		}
	}
	else {

	}
}

static void video_write_word(uint32_t address, uint16_t data) {

	if (address < VIDEO_FRAMEBUFFER_START) {

		if (ISACTIVE) {
			log_println(LEVEL_INFO, TAG, "write to registers during active period");
		}

		uint8_t reg = GETVIDREG(address);
		if (reg == 1) {
			if (*(video_registers[reg]) & VIDEO_CONFIG_FLIP) {
				log_println(LEVEL_INFO, TAG, "surface 1");
			}
			else {
				log_println(LEVEL_INFO, TAG, "surface 0");
			}
		}

		*(video_registers[reg]) = data;

		// update the window rects
		if (reg == VIDEO_REG_POSX || reg == VIDEO_REG_POSY || reg == VIDEO_REG_WINHEIGHT || reg == VIDEO_REG_WINWIDTH
				|| reg == VIDEO_REG_WINX || reg == VIDEO_REG_WINY) {
			video_updaterects();
		}
	}

	else if (address < VIDEO_COMPOSITINGBUFFER_START) {
		if (SDL_MUSTLOCK(FRONTSURFACE)) {
			SDL_LockSurface(FRONTSURFACE);
		}
		*((uint16_t*) FRONTSURFACE->pixels + ((address - VIDEO_FRAMEBUFFER_START) / 2)) = data;
		vramtouched = true;
		if (SDL_MUSTLOCK(FRONTSURFACE)) {
			SDL_UnlockSurface(FRONTSURFACE);
		}
	}
	else {

	}

}

static uint8_t video_read_byte(uint32_t address) {
	return 0;
}

static uint16_t video_read_word(uint32_t address) {

	if (address < VIDEO_FRAMEBUFFER_START) {
		uint8_t reg = GETVIDREG(address);
		return *(video_registers[reg]);
	}

	printf("invalid read\n");

	return 0;

}

static void videocard_irqack() {
	board_lower_interrupt(&videocard);
}

void videocard_setosd(SDL_Surface* s) {
	osd = s;
}

void videocard_refresh() {

	if (!vramtouched)
		return;

	// if the current window covers the whole surface don't bother clearing it.
	if (!((region.x == 0) && (region.y == 0) && (region.w == VIDEO_WIDTH) && (region.h == VIDEO_HEIGHT))) {
		SDL_FillRect(screen, NULL, 0x0);
	}

	SDL_BlitSurface(BACKSURFACE, &region, screen, &window);
	if (osd) {
		SDL_BlitSurface(osd, NULL, screen, NULL);
	}
	SDL_Flip(screen);
	vramtouched = false;
	//
}

static int videocard_bestcasecycles() {

	return ((VIDEO_WIDTH + VIDEO_HBLANKPERIOD) * VIDEO_VBLANKPERIOD) * VIDEO_MACHINECLOCKSPERPIXELCLOCK;

}

static int videocard_cyclesleft() {

	if (line < VIDEO_HEIGHT) {
		if (pixel < VIDEO_WIDTH) {
			int pixelsuntilhblank = VIDEO_WIDTH - pixel;
			//log_println(LEVEL_INFO, TAG, "pixels until hblank %d", pixelsuntilhblank);
			return pixelsuntilhblank * VIDEO_MACHINECLOCKSPERPIXELCLOCK;
		}
		else if (pixel >= VIDEO_WIDTH) {
			int pixelsuntilexitinghblank = (VIDEO_WIDTH + VIDEO_HBLANKPERIOD) - pixel;
			//log_println(LEVEL_INFO, TAG, "pixels until exiting hblank %d", pixelsuntilexitinghblank);
			return pixelsuntilexitinghblank * VIDEO_MACHINECLOCKSPERPIXELCLOCK;
		}
	}

	int linesuntilexitingvblank = (VIDEO_HEIGHT + VIDEO_VBLANKPERIOD) - line;
	//log_println(LEVEL_INFO, TAG, "lines until exiting vblank %d", linesuntilexitingvblank);
	return (linesuntilexitingvblank * (VIDEO_WIDTH + VIDEO_HBLANKPERIOD)) * VIDEO_MACHINECLOCKSPERPIXELCLOCK;

}

const card videocard = { "VIDEO CARD", //
		video_init, //
		video_dispose, //
		NULL, //
		video_tick, //
		videocard_irqack, //
		NULL, //
		video_validaddress, //
		video_read_byte, //
		video_read_word, //
		NULL, //
		video_write_byte, //
		video_write_word, //
		NULL, //
		NULL, //
		videocard_bestcasecycles, //
		videocard_cyclesleft //
		};
