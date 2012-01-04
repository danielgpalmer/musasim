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

#define WANTSOUNDFUNC
#include "soundcard.h"
#include "soundregistermasks.h"
#include "../util.h"
#include "../../utils.h"
#include "../../logging.h"
#include "../../sim.h"

#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <SDL.h>

static char TAG[] = "sound";

static uint8_t* sampleram;
#define BUFFERSIZE (5000 * 4)
static int16_t* audiobuffer;

static channel channels[TOTALCHANNELS];
static channel channelslatched[TOTALCHANNELS];

static uint32_t channelregisterbase;
static uint32_t channelbases[TOTALCHANNELS];

static void soundcard_sdlcallback(void* unused, uint8_t *stream, int len) {

	//static int audiopos = 0;

	for (int i = 0; i < len; i++) {
		*stream++ = ((uint8_t*) audiobuffer)[i];
	}

}

static bool active = false;

static void soundcard_init() {

	sampleram = malloc(SAMPLETOTAL);
	if (sampleram == NULL) {
		log_println(LEVEL_DEBUG, TAG, "sample ram malloc failed");
	}

	audiobuffer = malloc(BUFFERSIZE);
	if (audiobuffer == NULL) {
		log_println(LEVEL_DEBUG, TAG, "audiobuffer malloc failed");
	}

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

	soundcard_channelbases(channelbases, channelregisterbase);

	//FIXME this is just pasted from the docs
	SDL_AudioSpec fmt;
	fmt.freq = RATE;
	fmt.format = AUDIO_S16SYS;
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

static void soundcard_dump_config(audiochannel* chan) {
	log_println(LEVEL_DEBUG, TAG, "config 0x%04x, pointer 0x%04x, len 0x%04x, pos 0x%04x", chan->config,
			chan->samplepointer, chan->samplelength, chan->samplepos);

}
static void soundcard_dispose() {
	SDL_CloseAudio();
	free(sampleram);
	free(audiobuffer);
}

static void soundcard_tick() {

	static unsigned int audiobufferhead = 0;
	static int scaler = 0;

	if (scaler < TICKSPERSAMPLE) {
		scaler++;
		return;
	}

	scaler = 0;

	masterchannel* master = &(channels[0].master);

	if (master->config && SOUND_CHANNEL_ENABLED) { // sound is enabled

		log_println(LEVEL_DEBUG, TAG, "head %d", audiobufferhead);

		SDL_LockAudio();

		for (int i = 1; i < TOTALCHANNELS; i++) {
			if (channels[i].audio.config & SOUND_CHANNEL_ENABLED) {

				// channel ran out of samples.. so latch it again
				audiochannel* chan = &(channelslatched[i].audio);
				if (chan->samplepos == chan->samplelength) {
					//log_println(LEVEL_DEBUG, TAG, "latching channel");
					channelslatched[i] = channels[i];
					chan = &(channelslatched[i].audio);
				}

				int page = (chan->config & SOUND_CHANNEL_PAGE) >> SOUND_CHANNEL_PAGE_SHIFT;
				uint32_t pageoffset = SAMPLEPAGESIZE * page;
				uint32_t sampleoffset = pageoffset + chan->samplepointer + (chan->samplepos * 2);

				// LEFT
				if (chan->config & SOUND_CHANNEL_LEFT) {
					audiobuffer[audiobufferhead] = READ_WORD(sampleram, sampleoffset);
				}

				// RIGHT
				if (chan->config & SOUND_CHANNEL_RIGHT) {
					audiobuffer[audiobufferhead + 1] = READ_WORD(sampleram, sampleoffset);
				}

				// chan is all out of samples..
				chan->samplepos++;
				if (chan->samplepos == chan->samplelength) {
					if (chan->config & SOUND_CHANNEL_INTERRUPT) {
						chan->config |= SOUND_CHANNEL_RELOAD;
						// fire interrupt
					}
				}
				//soundcard_dump_config(chan);
			}

		}

		audiobufferhead += 4;
		// wraparound
		if (audiobufferhead >= BUFFERSIZE) {
			audiobufferhead = 0;
		}

		SDL_UnlockAudio();
	}
}

static uint16_t* soundcard_decodereg(uint32_t address) {

	int channelnum = (address & 0x78) >> 3;
	int reg = address - channelbases[channelnum];
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
			default:
				log_println(LEVEL_DEBUG, TAG, "invalid register %d", reg);
				return NULL;
		}

	}
}

static uint16_t soundcard_read_word(uint32_t address) {

	if (address < channelregisterbase) {
		return READ_WORD(sampleram, address);
	}
	else {
		uint16_t* reg = soundcard_decodereg(address);
		if (reg != NULL) {
			return *reg;
		}
	}

	return 0;
}

static void soundcard_write_word(uint32_t address, uint16_t value) {

	if (address < channelregisterbase) {
		WRITE_WORD(sampleram, address, value);
	}
	else {
		uint16_t* reg = soundcard_decodereg(address);
		if (reg != NULL) {
			*reg = value;
			for (int i = 1; i < NUMAUDIOCHANNELS; i++) {
				soundcard_dump_config(&(channels[i].audio));
			}
		}
	}

}

const card soundcard = { "SOUND CARD", soundcard_init, soundcard_dispose, soundcard_tick, NULL, NULL, NULL,
		soundcard_read_word, NULL, NULL, soundcard_write_word, NULL };
