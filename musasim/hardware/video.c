#include <stdint.h>
#include <stdbool.h>
#include <unistd.h>
#include <glib.h>
#include <SDL/SDL.h>

#include "common.h"
#include "video.h"

uint16_t flags;
uint16_t config;
uint16_t clearcolour;
uint16_t pixel = 0;
uint16_t line = 0;

uint16_t* video_registers[] = { &flags, &config, &clearcolour, &pixel, &line };

#define PIXELFORMAT 16
#define PIXELSIZE (PIXELFORMAT/8)
#define WIDTH 480
#define HEIGHT 272
#define HBLANKPERIOD 20
#define VBLANKPERIOD 28

//MAP FORMATS

// NORMAL	1 byte index
// PACKED   [nibble - nibble] packed index

// registers

#define FLAG_VBLANK 1
#define FLAG_HBLANK 1 << 1

#define MODE_DISABLED 0
#define MODE_BITMAP 1
#define MODE_TILED 2
#define MODE_MASK 0b11

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
uint32_t memoryend;
uint32_t currentaddress = 0;

GThread* thread;
GError* threaderr = NULL;

void video_threadfunc(void* data) {

	SDL_Event event;

	while (event.type != SDL_QUIT) {

		SDL_PollEvent(&event);
		usleep(250);

	}

	request_exit();

	g_thread_exit(NULL);
}

void video_init() {

	printf("video_init()\n");

	SDL_Init(SDL_INIT_EVERYTHING);
	screen = SDL_SetVideoMode(WIDTH, HEIGHT, PIXELFORMAT, SDL_SWSURFACE);

	printf("Created surface; %d x %d pixels @ %dBPP\n", screen->w, screen->h, screen->format->BitsPerPixel);

	pixels = (void*) screen->pixels;
	memoryend = ((screen->w * screen->h) * PIXELSIZE);

	SDL_EventState(SDL_ACTIVEEVENT, SDL_IGNORE);
	SDL_EventState(SDL_MOUSEMOTION, SDL_IGNORE);
	SDL_EventState(SDL_MOUSEBUTTONDOWN, SDL_IGNORE);
	SDL_EventState(SDL_MOUSEBUTTONUP, SDL_IGNORE);
	SDL_EventState(SDL_JOYAXISMOTION, SDL_IGNORE);
	SDL_EventState(SDL_JOYBALLMOTION, SDL_IGNORE);
	SDL_EventState(SDL_JOYHATMOTION, SDL_IGNORE);
	SDL_EventState(SDL_JOYBUTTONDOWN, SDL_IGNORE);
	SDL_EventState(SDL_JOYBUTTONUP, SDL_IGNORE);

	if ((thread = g_thread_create((GThreadFunc) video_threadfunc, NULL, TRUE, &threaderr)) == NULL)
	{
		printf("Thread create failed: %s!!\n", threaderr->message);
		g_error_free(threaderr);
	}
}

void video_quit() {

	printf("video_quit()\n");

}

bool validaddress(uint32_t address) {
	if (address < (memoryend + sizeof(video_registers))) {
		return true;
	}

	printf("0x%x is not a valid address\n", address);

	return false;
}

void video_tick() {

	//int mode = config & MODE_MASK;

	pixel++;

	if (pixel == WIDTH) {
		flags |= FLAG_HBLANK;
		//printf("HBLANK\n");
	}

	else if (pixel > WIDTH + HBLANKPERIOD) {

		flags &= !FLAG_HBLANK;
		pixel = 0;

		line++;

		if (line == HEIGHT) {
			printf("VBLANK\n");
			flags |= FLAG_VBLANK;
		}

		else if (line > HEIGHT + VBLANKPERIOD) {

			flags &= !FLAG_VBLANK;
			line = 0;

			//if (mode == MODE_BITMAP) {
			SDL_Flip(screen);
			//}

		}
	}
}

void video_write_byte(uint32_t address, uint8_t data) {

	if (!validaddress(address)) {
		return;
	}

	//if (DEBUG) {
	printf("writing pixel value %x to %x\n", data, address);
	//}

	if (SDL_MUSTLOCK(screen)) {
		SDL_LockSurface(screen);
	}
	*((uint8_t*) screen->pixels + address) = data;
	if (SDL_MUSTLOCK(screen)) {
		SDL_UnlockSurface(screen);
	}
}

void video_write_word(uint32_t address, uint16_t data) {

	if (!validaddress(address)) {
		return;
	}

	if (DEBUG) {
		printf("writing pixel value %x to %x\n", data, address);
	}

	if (address % 2 != 0) {
		printf("Word writes must be aligned\n");
		return;
	}

	//if (address < memoryend) {
	if (SDL_MUSTLOCK(screen)) {
		SDL_LockSurface(screen);
	}

	if (DEBUG) {
		printf("Raw pixel write\n");
	}
	*((uint16_t*) screen->pixels + (address / 2)) = data;

	if (SDL_MUSTLOCK(screen)) {
		SDL_UnlockSurface(screen);
	}

	SDL_Flip(screen);
	//}
	//else {
	//
	//	if (registerwritecheck()) {
	//		*video_registers[address - memoryend] = data;
//
	//	}
	//	else {
	//		printf("Ignored write to registers during active period\n");
	//	}
//
	//	dumpregs();
	//}

}

uint8_t video_read_byte(uint32_t address) {

	if (!validaddress(address)) {
		return 0;
	}

	return 0;

}

uint16_t video_read_word(uint32_t address) {

	if (!validaddress(address)) {
		return 0;
	}

	return 0;

}

bool registerwritecheck() {

	if ((flags & FLAG_VBLANK) == FLAG_VBLANK) {
		return true;
	}

	return false;
}

void dumpregs() {

	printf("-- REGS --\n");

	if ((flags & FLAG_VBLANK) == FLAG_VBLANK) {
		printf("VBlank\n");
	}

}
