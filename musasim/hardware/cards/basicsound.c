/*
 * basicsound.c
 *
 *  Created on: 22 May 2012
 *      Author: daniel
 */

#include <stdint.h>
#include <stdbool.h>
#include "basicsound.h"
#include "../../logging.h"
#include "../board.h"
#include "blipbuf/blip_buf.h"
#include "../../sim.h"
#include <SDL.h>

static const char TAG[] = "basicsound";

static uint16_t divisor = 0;
static uint16_t latcheddivisor = 0;
static uint16_t counter = 0;

static bool inited = false;

static blip_t* buf;

#define SAMPLERATE 22000

static void basicsound_sdlcallback(void* unused, uint8_t *stream, int len) {
	if (inited) {
		blip_read_samples(buf, (short*) stream, len, 0);

	}
}

static void basicsound_init() {
	log_println(LEVEL_DEBUG, TAG, "basicsound init");
	buf = blip_new(SAMPLERATE / 5);
	if (buf == NULL) {
		log_println(LEVEL_WARNING, TAG, "failed to create buffer");
		return;
	}

	blip_set_rates(buf, SIM_CPUCLOCK, SAMPLERATE);

	SDL_AudioSpec fmt;
	fmt.freq = SAMPLERATE;
	fmt.format = AUDIO_S16SYS; // BE samples will get converted to the local format
	fmt.channels = 1;
	fmt.samples = 64;
	fmt.callback = basicsound_sdlcallback;
	fmt.userdata = NULL;

	if (SDL_OpenAudio(&fmt, NULL) == 0) {
		SDL_PauseAudio(0);
		log_println(LEVEL_DEBUG, TAG, "SDL output is now active");
	}
	else {
		blip_delete(buf);
		return;
	}

	inited = true;

}

static void basicsound_dispose() {

	if (buf != NULL) {
		blip_delete(buf);
	}

	SDL_CloseAudio();

}

static bool basicsound_validaddress(uint32_t address) {
	return true;
}

static int time; /* clock time of next delta */
static int phase = +1; /* +1 or -1 */
static int volume = 10000;
static int amp; /* current amplitude in delta buffer */

static void basicsound_tick(int cyclesexectuded) {

	if (!inited)
		return;

	for (time = 0; time < cyclesexectuded; time++) {
		counter += 1;
		if (counter == latcheddivisor) {
			counter = 0;
			latcheddivisor = divisor;
			int delta = phase * volume - amp;
			amp += delta;
			blip_add_delta(buf, time, delta);
			phase = -phase;
		}
	}

	blip_end_frame(buf, cyclesexectuded);

}

static void basicsound_write_word(uint32_t address, uint16_t data) {
	divisor = data;
}

static uint16_t basicsound_read_word(uint32_t address) {
	return divisor;
}

const card basicsoundcard = { "BASIC SOUND CARD", //
		basicsound_init, //init
		basicsound_dispose, // dispose
		NULL, //
		basicsound_tick, // tick
		NULL, //
		NULL, //
		basicsound_validaddress, //
		NULL, //
		basicsound_read_word, //
		NULL, //
		NULL, //
		basicsound_write_word, //
		NULL, //
		NULL, //
		NULL, //
		NULL };
