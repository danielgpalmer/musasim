#include <stdint.h>
#include <stdbool.h>
#include <unistd.h>
#include <glib.h>
#include <SDL/SDL.h>

#define PIXELFORMAT 16
#define PIXELSIZE (PIXELFORMAT/8)
#define WIDTH 480
#define HEIGHT 272
#define HBLANKPERIOD 20
#define VBLANKPERIOD 28

#define TILESIZE 8
#define BGMAPHEIGHT 50
#define BGMAPWIDTH 80

// registers
uint8_t mode;
uint8_t clearcolour;
uint16_t pixel = 0;
uint16_t line = 0;
//

uint8_t bg0map[BGMAPWIDTH][BGMAPHEIGHT];
uint8_t bg1map[BGMAPWIDTH][BGMAPHEIGHT];

bool DEBUG = false;

SDL_Surface* screen = NULL;

void* pixels;
uint32_t endaddress;
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
	endaddress = ((screen->w * screen->h) * PIXELSIZE) - PIXELSIZE;

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
	if (address <= endaddress) {
		return true;
	}

	printf("0x%x is not a valid address\n", address);

	return false;
}

void video_tick() {

	pixel++;
	if (pixel > WIDTH + HBLANKPERIOD) {
		pixel = 0;
		line++;
		//printf("HBLANK\n");
		if (line > HEIGHT + VBLANKPERIOD) {
			line = 0;
			SDL_Flip(screen);
			printf("VBLANK\n");

		}
	}

}

void video_write_byte(uint32_t address, uint8_t data) {

	if (!validaddress(address)) {
		return;
	}

	if (SDL_MUSTLOCK(screen)) {
		SDL_LockSurface(screen);
	}
	*((uint8_t*) screen->pixels + address) = data;
	if (SDL_MUSTLOCK(screen)) {
		SDL_UnlockSurface(screen);
	}
}

void video_write_word(uint32_t address, uint16_t data) {

	if (DEBUG) {
		printf("writing pixel value %x to %x\n", data, address);
	}

	if (!validaddress(address)) {
		return;
	}

	if (address % 2 != 0) {
		printf("Word writes must be aligned\n");
		return;
	}

	if (SDL_MUSTLOCK(screen)) {
		SDL_LockSurface(screen);
	}

	*((uint16_t*) screen->pixels + (address / 2)) = data;

	if (SDL_MUSTLOCK(screen)) {
		SDL_UnlockSurface(screen);
	}
}

uint8_t video_read_byte(uint32_t address) {

	if (!validaddress(address)) {
		return 0;
	}

	return 0;

}

uint16_t video_read_word(uint32_t address, uint16_t data) {

	if (!validaddress(address)) {
		return 0;
	}

	return 0;

}

