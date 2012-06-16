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
#include <sound_registermasks.h>
#include "soundcard.h"
#include "../util.h"
#include "../board.h"
#include "../../utils.h"
#include "../../logging.h"
#include "../../sim.h"

#include <assert.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <SDL.h>

static char TAG[] = "sound";

#define VOLLEFT(volreg) ((volreg & 0xFF00) >> 8)
#define VOLRIGHT(volreg) (volreg & 0x00FF)

// "Hardware"
static uint8_t* sampleram;
static channel channels[TOTALCHANNELS];
static channel channelslatched[TOTALCHANNELS];
static uint32_t channelregisterbase;
static uint32_t channelbases[TOTALCHANNELS];
//

// This is a ring buffer for the computed audio
#define OUTPUTCHANNELS 2
#define BUFFER ((10 * RATE) * OUTPUTCHANNELS) // 10s
#define SAMPLESIZE (sizeof(int16_t))
#define BUFFERSIZE (BUFFER * SAMPLESIZE) // FUDGE
static int16_t* audiobuffer;
static unsigned int audiobufferhead = 0;
static unsigned int audiobuffertail = 0;
//

static void soundcard_sdlcallback(void* unused, uint8_t *stream, int len) {

	for (int i = 0; i < len; i++) {

		if (audiobuffertail == audiobufferhead) {
			log_println(LEVEL_INFO, TAG, "audio underrun");
			*stream++ = 0x00;
		}

		*stream++ = *((uint8_t*) audiobuffer + audiobuffertail);
		audiobuffertail++;
		if (audiobuffertail == BUFFERSIZE) {
			audiobuffertail = 0;
		}
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
	for (int i = 1; i < TOTALCHANNELS; i++) {
		log_println(LEVEL_INFO, TAG, "Channel %d is at 0x%08x", i - 1, channelbases[i]);
	}

	//FIXME this is just pasted from the docs
	SDL_AudioSpec fmt;
	fmt.freq = RATE;
	fmt.format = AUDIO_S16SYS; // BE samples will get converted to the local format
	fmt.channels = OUTPUTCHANNELS;
	fmt.samples = 512;
	fmt.callback = soundcard_sdlcallback;
	fmt.userdata = NULL;

	if (SDL_OpenAudio(&fmt, NULL) == 0) {
		active = true;
		SDL_PauseAudio(0);
		log_println(LEVEL_DEBUG, TAG, "SDL output is now active");
	}

}

static void soundcard_dump_config(int channel) {

	if (channel == 0) {
		masterchannel* chan = &(channels[channel].master);
		log_println(LEVEL_DEBUG, TAG, "master channel, config 0x%04x, volume left %d right %d", chan->config,
				VOLLEFT(chan->volume), VOLRIGHT(chan->volume));
	}
	else {
		audiochannel* chan = &(channels[channel].audio);
		log_println(LEVEL_DEBUG, TAG,
				"channel %d, config 0x%04x, pointer 0x%04x, len 0x%04x, pos 0x%04x, volume left %d right %d",
				channel - 1, chan->config, chan->samplepointer, chan->samplelength, chan->samplepos,
				VOLLEFT(chan->volume), VOLRIGHT(chan->volume));
	}
}
static void soundcard_dispose() {
	SDL_CloseAudio();
	free(sampleram);
	free(audiobuffer);
}

static int16_t soundcard_mixsamples(int16_t sample1, int16_t sample2, uint8_t volume) {

	float fvolume = volume / UINT8_MAX;

	int32_t mixed = sample1 + (sample2 * fvolume);

	// clamp
	if (mixed > INT16_MAX) {
		mixed = INT16_MAX;
	}
	else if (mixed < INT16_MIN) {
		mixed = INT16_MIN;
	}

	return (int16_t) mixed;
}

static void soundcard_tick() {

	static unsigned int bufferindex = 0;
	static int scaler = 0;

	if (!active) {
		return;
	}

	if (scaler < TICKSPERSAMPLE) { // Fudge
		scaler++;
		return;
	}
	scaler = 0;

	masterchannel* master = &(channels[0].master);

	if (master->config && SOUND_CHANNEL_ENABLED) { // sound is enabled

		SDL_LockAudio();

		audiobuffer[bufferindex] = 0;
		audiobuffer[bufferindex + 1] = 0;

		for (int i = 1; i < TOTALCHANNELS; i++) {
			if (channels[i].audio.config & SOUND_CHANNEL_ENABLED) {

				// channel ran out of samples.. so latch it again
				audiochannel* chan = &(channelslatched[i].audio);
				if (chan->samplepos == chan->samplelength) {
					channelslatched[i] = channels[i];
					chan = &(channelslatched[i].audio);
				}

				int page = (chan->config & SOUND_CHANNEL_PAGE) >> SOUND_CHANNEL_PAGE_SHIFT;
				uint32_t pageoffset = SAMPLEPAGESIZE * page;
				uint32_t sampleoffset = pageoffset + chan->samplepointer + (chan->samplepos * 2);

				// LEFT
				if (chan->config & SOUND_CHANNEL_LEFT) {
					audiobuffer[bufferindex] = soundcard_mixsamples(audiobuffer[bufferindex],
							READ_WORD(sampleram, sampleoffset), VOLLEFT(chan->volume));
				}

				// RIGHT
				if (chan->config & SOUND_CHANNEL_RIGHT) {
					audiobuffer[bufferindex + 1] = soundcard_mixsamples(audiobuffer[bufferindex + 1],
							READ_WORD(sampleram, sampleoffset), VOLRIGHT(chan->volume));
				}

				// chan is all out of samples..
				chan->samplepos++;
				if (chan->samplepos == chan->samplelength) {
					if (chan->config & SOUND_CHANNEL_INTERRUPT) {
						chan->config |= SOUND_CHANNEL_RELOAD;
						board_raise_interrupt(&soundcard);
					}
				}
			}

		}

		// Adjust the output to the master volume.
		audiobuffer[bufferindex] *= VOLLEFT(master->volume) / UINT8_MAX;
		audiobuffer[bufferindex + 1] *= VOLLEFT(master->volume) / UINT8_MAX;

		bufferindex += OUTPUTCHANNELS;

		// wraparound
		if (bufferindex == BUFFER) {
			bufferindex = 0;
		}

		audiobufferhead = bufferindex * SAMPLESIZE;

		// we have caught up with the output
		if (audiobufferhead == audiobuffertail) {
			log_println(LEVEL_INFO, TAG, "audio buffer has overflown");
		}

		SDL_UnlockAudio();
	}
}

static uint16_t* soundcard_decodereg(uint32_t address) {

	unsigned int channelnum = (address & 0xF0) >> 4;
	uint32_t base = channelbases[channelnum];
	unsigned int reg = address - base;

	log_println(LEVEL_DEBUG, TAG, "offset is %d, 0x%x", reg, base);

	if (reg != 0) {
		reg /= 2;
	}

	if (channelnum == 0) {
		masterchannel* chan = &(channels[channelnum].master);
		switch (reg) {
			case 0:
				return &(chan->config);
			case 1:
				return &(chan->volume);
			default:
				return NULL;
		}
	}
	else {
		audiochannel* chan = &(channels[channelnum].audio);
		switch (reg) {
			case 0:
				return &(chan->config);
			case 1:
				return &(chan->volume);
			case 2:
				return &(chan->samplepointer);
			case 3:
				return &(chan->samplelength);
			case 4:
				return &(chan->samplepos);
			default:
				log_println(LEVEL_DEBUG, TAG, "invalid register %d, address was 0x%08x, chan is %d", reg, address,
						channelnum - 1);
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
			for (int i = 0; i < NUMAUDIOCHANNELS; i++) {
				soundcard_dump_config(i);
			}
		}
	}

}

static void soundcard_irqack() {
	board_lower_interrupt(&soundcard);
}

static bool soundcard_validaddress(uint32_t address) {
	return true;
}

const card soundcard = { "SOUND CARD", //
		soundcard_init, //
		soundcard_dispose, //
		NULL, //
		soundcard_tick, //
		soundcard_irqack, //
		NULL, //
		soundcard_validaddress, //
		NULL, //
		soundcard_read_word, //
		NULL, //
		NULL, //
		soundcard_write_word, //
		NULL, //
		NULL //
		};
