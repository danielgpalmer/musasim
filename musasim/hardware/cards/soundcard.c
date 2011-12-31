/*
 * I think this is going to work a bit like the Amiga Paula[1] ..
 * The basic idea is that you spam the registers for each channel with the location/length/rate of a sample and
 * it starts playing when the currently playing sample stops. An interrupt fires when the registers are being latched
 * maybe there will be fifos in front of the registers so that you can pump stuff in.
 * Unlike paula I think I'm going for 8 channels and sixteen bit signed samples (could be crazy.. I've never done this before)
 * Sample RAM will be on the card itself.. so no access needs to happen on the main bus .. there will be a window for writing the
 * sample ram.
 *
 *
 * 1	-	http://amigadev.elowar.com/read/ADCD_2.1/Hardware_Manual_guide/node0000.html
 * 			Online copy of the amiga hardware reference
 */

#include "soundcard.h"
#include "../../utils.h"
#include "../../logging.h"

#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <SDL.h>

static char TAG[] = "sound";

#define NUMCHANNELS 8

#define SAMPLEPAGES 4
#define SAMPLEPAGESIZE 0xFFFF
#define SAMPLETOTAL (SAMPLEPAGESIZE * SAMPLEPAGES)
static uint8_t* sampleram;

/*
 *	15	4	3	2	1	0
 *	i	[R	]	S	I	E
 *
 *	E - Enabled
 *	I - Interrupt
 *	S - Side
 *	R - Rate, divisor of the master clock
 *	i - Interrupt fired
 */

typedef struct {
	uint16_t config;
	uint16_t samplepointer;
	uint16_t samplelength;
	uint16_t samplepos;

} channel;

static channel channels[NUMCHANNELS];
static channel channelslatched[NUMCHANNELS];

static uint32_t channelregisterbase;
static uint32_t channelbases[NUMCHANNELS];

static void soundcard_sdlcallback(void* unused, uint8_t *stream, int len) {

	// TODO mixing here

}

static bool active = false;

static void soundcard_init() {

	sampleram = malloc(SAMPLETOTAL);

	for (int i = 0; i < NUMCHANNELS; i++) {
		channel* chan = &(channels[i]);
		chan->samplepointer = 0;
		chan->samplelength = 0;
	}

	channelregisterbase = utils_nextpow(SAMPLETOTAL);

	log_println(LEVEL_DEBUG, TAG, "registers start at 0x%08x", channelregisterbase);

	int registerspaddedsize = utils_nextpow(sizeof(channel));
	for (int i = 0; i < NUMCHANNELS; i++) {
		channelbases[i] = channelregisterbase + (registerspaddedsize * i);
		log_println(LEVEL_DEBUG, TAG, "channel %d base is at 0x%08x ", i, channelbases[i]);
	}

	//FIXME this is just pasted from the docs
	SDL_AudioSpec fmt;
	fmt.freq = 22050;
	fmt.format = AUDIO_S16;
	fmt.channels = 2;
	fmt.samples = 512;
	fmt.callback = soundcard_sdlcallback;
	fmt.userdata = NULL;

	if (SDL_OpenAudio(&fmt, NULL) == 0) {
		active = true;
		SDL_PauseAudio(0);
		log_println(LEVEL_DEBUG, TAG, "SDL output is now active");
	}

}

static void soundcard_dispose() {
	SDL_CloseAudio();
	free(sampleram);

}

static void soundcard_tick() {

	for (int i = 0; i < NUMCHANNELS; i++) {

		channelslatched[i] = channels[i]; // when the channel should be latched

		channel* chan = &(channelslatched[i]);
		chan->samplepos++;
	}

}

static uint16_t* soundcard_decodereg(uint32_t address) {

	if (address < channelregisterbase) {
		//return &sampleram[a]
	}
	else {
		int channelnum = 0;
		int reg = address - channelregisterbase;
		if (reg != 0) {
			reg /= 2;
		}
		switch (reg) {
			case 0:
				return &(channels[channelnum].config);
			case 1:
				return &(channels[channelnum].samplepointer);
			case 2:
				return &(channels[channelnum].samplelength);
			case 3:
				return &(channels[channelnum].samplepos);
		}

	}

	return NULL;

}

static uint16_t soundcard_read_word(uint32_t address) {

	soundcard_decodereg(address);
	return 0;
}

static void soundcard_write_word(uint32_t address, uint16_t value) {

	soundcard_decodereg(address);

}

const card soundcard = { "SOUND CARD", soundcard_init, soundcard_dispose, soundcard_tick, NULL, NULL, NULL,
		soundcard_read_word, NULL, NULL, soundcard_write_word, NULL };
