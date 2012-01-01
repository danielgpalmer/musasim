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
#include "soundregistermasks.h"
#include "../util.h"
#include "../../utils.h"
#include "../../logging.h"

#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <SDL.h>

static char TAG[] = "sound";

#define NUMAUDIOCHANNELS 8
#define TOTALCHANNELS (NUMAUDIOCHANNELS + 1)

#define SAMPLEPAGES 4
#define SAMPLEPAGESIZE 0xFFFF
#define SAMPLETOTAL (SAMPLEPAGESIZE * SAMPLEPAGES)
static uint8_t* sampleram;
#define BUFFERSIZE (SAMPLETOTAL * 16)
static uint8_t* audiobuffer;
static unsigned int audiobufferhead = 0;

/*
 *	15	5	4	3	2	1	0
 *	i	[R	]	l	r	I	E
 *
 *	E - Enabled
 *	I - Interrupt
 *	r - Right Side
 *	l - Left Side
 *	R - Rate, divisor of the master clock
 *	i - Interrupt fired
 */

typedef struct {
	uint16_t config;
} masterchannel;

typedef struct {
	uint16_t config;
	uint16_t samplepointer;
	uint16_t samplelength;
	uint16_t samplepos;

} audiochannel;

typedef union {
	masterchannel master;
	audiochannel audio;
} channel;

static channel channels[TOTALCHANNELS];
static channel channelslatched[TOTALCHANNELS];

static uint32_t channelregisterbase;
static uint32_t channelbases[TOTALCHANNELS];

static void soundcard_sdlcallback(void* unused, uint8_t *stream, int len) {

	// TODO mixing here
	//log_println(LEVEL_DEBUG, TAG, "sdlcallback len %d", len);

	SDL_MixAudio(stream, audiobuffer, len, SDL_MIX_MAXVOLUME);
}

static bool active = false;

static void soundcard_init() {

	sampleram = malloc(SAMPLETOTAL);
	audiobuffer = malloc(BUFFERSIZE);

	//FILE* rand = fopen("/dev/urandom", "r");
	//for (int i = 0; i < SAMPLETOTAL; i += 4) {
	//	fread(&(sampleram[i]), 1, 2, rand);
	//}
	//fclose(rand);

	for (int i = 0; i < TOTALCHANNELS; i++) {
		if (i == 0) {
			masterchannel* chan = &(channels[i].master);
			chan->config = 0;
		}
		else {
			audiochannel* chan = &(channels[i].audio);
			chan->samplepointer = 0;
			chan->samplelength = 0;
		}
	}

	channelregisterbase = utils_nextpow(SAMPLETOTAL);

	log_println(LEVEL_DEBUG, TAG, "registers start at 0x%08x", channelregisterbase);

	int registerspaddedsize = utils_nextpow(sizeof(audiochannel));
	for (int i = 0; i < TOTALCHANNELS; i++) {
		channelbases[i] = channelregisterbase + (registerspaddedsize * i);
		log_println(LEVEL_DEBUG, TAG, "channel %d base is at 0x%08x ", i, channelbases[i]);
	}

	//FIXME this is just pasted from the docs
	SDL_AudioSpec fmt;
	fmt.freq = 22050;
	fmt.format = AUDIO_S16MSB;
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
	free(audiobuffer);
}

static void soundcard_tick() {

	masterchannel* master = &(channels[0].master);

	if (master->config && SOUND_CHANNEL_ENABLED) { // sound is enabled

		SDL_LockAudio();

		for (int i = 1; i < TOTALCHANNELS; i++) {
			if (channels[i].audio.config & SOUND_CHANNEL_ENABLED) {

				// channel ran out of samples.. so latch it again
				audiochannel* chan = &(channelslatched[i].audio);
				if (chan->samplepos == chan->samplelength) {
					channelslatched[i] = channels[i];
					chan = &(channelslatched[i].audio);
				}

				// LEFT
				if (chan->config & SOUND_CHANNEL_LEFT) {

				}

				// RIGHT
				if (chan->config & SOUND_CHANNEL_RIGHT) {

				}

				// chan is all out of samples..
				chan->samplepos++;
				if (chan->samplepos == chan->samplelength) {
					// fire interrupts here
				}
			}
		}

		audiobufferhead++;

		SDL_UnlockAudio();
	}
}

static uint16_t* soundcard_decodereg(uint32_t address) {

	int channelnum = 0;
	int reg = address - channelregisterbase;
	if (reg != 0) {
		reg /= 2;
	}

	if (channelnum == 0) {
		masterchannel* chan = &(channels[channelnum].master);
		return &(chan->config);
	}
	else {
		audiochannel* chan = &(channels[channelnum].audio);
		switch (reg) {
			case 0:
				return &(chan->config);
			case 1:
				return &(chan->samplepointer);
			case 2:
				return &(chan->samplelength);
			case 3:
				return &(chan->samplepos);
		}
	}

	return NULL;

}

static uint16_t soundcard_read_word(uint32_t address) {

	if (address < channelregisterbase) {
		return READ_WORD(sampleram, address);
	}
	else {
		return *(soundcard_decodereg(address));
	}

}

static void soundcard_write_word(uint32_t address, uint16_t value) {

	if (address < channelregisterbase) {
		WRITE_WORD(sampleram, address, value);
	}
	else {
		*(soundcard_decodereg(address)) = value;
	}

}

const card soundcard = { "SOUND CARD", soundcard_init, soundcard_dispose, soundcard_tick, NULL, NULL, NULL,
		soundcard_read_word, NULL, NULL, soundcard_write_word, NULL };
