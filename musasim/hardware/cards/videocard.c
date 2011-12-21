#include <stdint.h>
#include <stdbool.h>
#include <unistd.h>
#include <SDL/SDL.h>

#include "../board.h"
#include "videocard.h"
#include "videoregistermasks.h"
#include "../../sim.h"
#include "../../logging.h"
#include "../../utils.h"

static const char TAG[] = "video";

uint16_t flags;
uint16_t config;
uint16_t clearcolour;
uint16_t pixel = 0;
uint16_t line = 0;

uint16_t* video_registers[] = { &flags, &config, &clearcolour, &pixel, &line };

#define HBLANKPERIOD 20
#define VBLANKPERIOD 28

//

// maps

// tile index 0 is special and is a nop

uint8_t bg0mapflags;
uint8_t bg1mapflags;
uint8_t bg0map[256];
uint8_t bg1map[256];

// TILE FORMATS

// SMALL 	8x8
// NORMAL	16x16
// SKINNY	8x16
// FAT		32x32

#define TILEFORMAT_SIZE_FAT 32

// Tile memory map
// 0x0 -- Tile format
// 0x1 -- Title data
// ..

#define BGTILEARRAYSIZE 255 * ((TILEFORMAT_SIZE_FAT/8) * TILEFORMAT_SIZE_FAT)

uint8_t bg0tileflags = 0;
uint8_t bg0tiles[BGTILEARRAYSIZE];

uint8_t bg1tileflags = 0;
uint8_t bg1tiles[BGTILEARRAYSIZE];

bool DEBUG = false;

SDL_Surface* screen = NULL;

void* pixels;
uint32_t currentaddress = 0;

uint32_t registersstart;
#define GETVIDREG(x) ( x = 0 ? 0 : (x & ~registersstart) / 2)

void video_init() {

	log_println(LEVEL_DEBUG, TAG, "video_init()");

	SDL_Init(SDL_INIT_EVERYTHING);
	screen = SDL_SetVideoMode(VIDEO_WIDTH, VIDEO_HEIGHT, VIDEO_PIXELFORMAT, SDL_SWSURFACE);

	log_println(LEVEL_INFO, TAG, "Created surface; %d x %d pixels @ %dBPP", screen->w, screen->h,
			screen->format->BitsPerPixel);

	pixels = (void*) screen->pixels;

	registersstart = utils_nextpow(VIDEO_MEMORYEND);
	log_println(LEVEL_DEBUG, TAG, "Memory size is 0x%x, registers start at 0x%x", VIDEO_MEMORYEND, registersstart);

}

void video_dispose() {

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

void video_tick() {

	int mode = config & VIDEO_CONFIG_MODE_MASK;

	if (mode == VIDEO_CONFIG_MODE_DISABLED) {
		return;
	}

	pixel++;

	if (pixel == VIDEO_WIDTH) {
		flags |= FLAG_HBLANK;
		if (config & VIDEO_CONFIG_ENHBINT) {
			board_raise_interrupt(&videocard);
		}
	}

	else if (pixel > VIDEO_WIDTH + HBLANKPERIOD) {

		flags &= !FLAG_HBLANK;
		pixel = 0;

		line++;

		if (line == VIDEO_HEIGHT) {
			flags |= FLAG_VBLANK;
			if (config & VIDEO_CONFIG_ENVBINT) {
				board_raise_interrupt(&videocard);
			}
		}

		else if (line > VIDEO_HEIGHT + VBLANKPERIOD) {

			flags &= ~FLAG_VBLANK;
			line = 0;

			//if (mode == MODE_BITMAP) {
			SDL_Flip(screen);
			//}

		}
	}
}

void video_write_byte(uint32_t address, uint8_t data) {

	if (!video_validaddress(address)) {
		return;
	}

	if (address < registersstart) {
		if (SDL_MUSTLOCK(screen)) {
			SDL_LockSurface(screen);
		}
		*((uint8_t*) screen->pixels + address) = data;
		if (SDL_MUSTLOCK(screen)) {
			SDL_UnlockSurface(screen);
		}
	}
}

void video_write_word(uint32_t address, uint16_t data) {

	if (!video_validaddress(address)) {
		return;
	}

	if (address < registersstart) {
		if (SDL_MUSTLOCK(screen)) {
			SDL_LockSurface(screen);
		}

		*((uint16_t*) screen->pixels + (address / 2)) = data;

		if (SDL_MUSTLOCK(screen)) {
			SDL_UnlockSurface(screen);
		}

		SDL_Flip(screen);
	}

	else {
		uint8_t reg = GETVIDREG(address);
		*video_registers[reg] = data;
		log_println(LEVEL_DEBUG, TAG, "Wrote to video register %d", reg);
	}

}

uint8_t video_read_byte(uint32_t address) {

	if (!video_validaddress(address)) {
		return 0;
	}

	return 0;

}

uint16_t video_read_word(uint32_t address) {

	if (!video_validaddress(address)) {
		return 0;
	}

	if (address >= registersstart) {
		uint8_t reg = GETVIDREG(address);
		log_println(LEVEL_DEBUG, TAG, "Read video register %d", reg);
		return *(video_registers[reg]);
	}

	return 0;

}

void dumpregs() {

	printf("-- REGS --\n");

	if ((flags & FLAG_VBLANK) == FLAG_VBLANK) {
		printf("VBlank\n");
	}

}

void videocard_irqack() {
	log_println(LEVEL_DEBUG, TAG, "videocard_irqack()");
	board_lower_interrupt(&videocard);
}

card videocard = { "VIDEO CARD", video_init, video_dispose, video_tick, videocard_irqack, NULL, video_read_byte,
		video_read_word, NULL, video_write_byte, video_write_word, NULL };
