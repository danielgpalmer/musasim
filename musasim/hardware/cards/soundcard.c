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
#define BUFFER ((1 * RATE) * OUTPUTCHANNELS) // 1s
#define SAMPLESIZE (sizeof(int16_t))
#define BUFFERSIZE (BUFFER * SAMPLESIZE) // FUDGE
static int16_t* audiobuffer;
static unsigned int audiobufferhead = 0;
static unsigned int audiobuffertail = 0;
//

static cardaddressspace* soundcard_setupaddressspace() {
	cardaddressspace* as = NULL;
	return as;
}

static void soundcard_sdlcallback(void* unused, uint8_t *stream, int len) {

	for (int i = 0; i < len; i++) {

		if (audiobuffertail == audiobufferhead) {
			log_println(LEVEL_DEBUG, TAG, "audio underrun");
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

static void soundcard_reset() {
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
}

static void soundcard_init() {

	sampleram = malloc(SAMPLETOTAL);
	if (sampleram == NULL ) {
		log_println(LEVEL_DEBUG, TAG, "sample ram malloc failed");
	}

	audiobuffer = malloc(BUFFERSIZE);
	if (audiobuffer == NULL ) {
		log_println(LEVEL_DEBUG, TAG, "audiobuffer malloc failed");
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
	fmt.samples = 1024;
	fmt.callback = soundcard_sdlcallback;
	fmt.userdata = NULL;

	if (SDL_OpenAudio(&fmt, NULL ) == 0) {
		active = true;
		SDL_PauseAudio(0);
		log_println(LEVEL_DEBUG, TAG, "SDL output is now active");
	}

}

static void soundcard_dump_config(int channel, bool latched) {

	if (channel == 0) {
		masterchannel* chan = &(channels[channel].master);
		log_println(LEVEL_DEBUG, TAG, "master channel, config 0x%04x, volume left %d right %d", chan->config,
				VOLLEFT(chan->volume), VOLRIGHT(chan->volume));
	}
	else {
		audiochannel* chan = latched ? &(channelslatched[channel].audio) : &(channels[channel].audio);
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

static void soundcard_tick(int cyclesexecuted) {

	static int carry = 0;

	static unsigned int bufferindex = 0;

	//if (!active) {
	//	return;
	//}
	int total = cyclesexecuted + carry;
	carry = total % TICKSPERSAMPLE;
	int ticks = total - carry;
	if (ticks == 0)
		return;

	//log_println(LEVEL_INFO, TAG, "ticks %d", ticks);

	for (int i = 0; i < (ticks / TICKSPERSAMPLE); i++) {

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
						//audiobuffer[bufferindex] = soundcard_mixsamples(audiobuffer[bufferindex],
						//		READ_WORD(sampleram, sampleoffset), VOLLEFT(chan->volume));
						audiobuffer[bufferindex] = READ_WORD(sampleram, sampleoffset);
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
			//audiobuffer[bufferindex] *= VOLLEFT(master->volume) / UINT8_MAX;
			//audiobuffer[bufferindex + 1] *= VOLLEFT(master->volume) / UINT8_MAX;

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
	//soundcard_dump_config(1, true);
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
				return NULL ;
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
				return NULL ;
		}

	}
}

static uint16_t soundcard_read_word(uint32_t address) {

	if (address < channelregisterbase) {
		return READ_WORD(sampleram, address);
	}
	else {
		uint16_t* reg = soundcard_decodereg(address);
		if (reg != NULL ) {
			return *reg;
		}
	}

	return 0;
}

static void soundcard_write_word(uint32_t address, uint16_t value) {

	if (address < channelregisterbase) {
		log_println(LEVEL_INFO, TAG, "Write to sample ram @ 0x%"PRIx32" value: 0x%"PRIx16, address, value);
		WRITE_WORD(sampleram, address, value);
	}
	else {
		log_println(LEVEL_INFO, TAG, "register write 0x%"PRIx32, address);
		uint16_t* reg = soundcard_decodereg(address);
		if (reg != NULL ) {
			*reg = value;
			for (int i = 0; i <= NUMAUDIOCHANNELS; i++) {
				soundcard_dump_config(i, false);
			}
		}
		else
			log_println(LEVEL_INFO, TAG, "bad address 0x%"PRIx32, address);
	}

}

static void soundcard_irqack() {
	board_lower_interrupt(&soundcard);
}

static bool soundcard_validaddress(uint32_t address) {
	return true;
}

int16_t* sound_getbuffer(unsigned int* head, unsigned int* len) {
	*head = audiobufferhead;
	*len = BUFFERSIZE / SAMPLESIZE;
	return audiobuffer;
}

static void soundcard_pause(bool paused) {
	if (paused)
		SDL_PauseAudio(1);
	else
		SDL_PauseAudio(0);
}

const card soundcard = { //
		"SOUND CARD", //
				soundcard_init, //
				soundcard_dispose, //
				soundcard_reset, //
				soundcard_tick, //
				soundcard_pause, // pause
				soundcard_irqack, //
				NULL, //
				NULL, //
				soundcard_validaddress, //
				NULL, //
				NULL, //
				soundcard_read_word, //
				NULL, //
				NULL, //
				soundcard_write_word, //
				NULL, //
				NULL, //
				NULL, //
				NULL, //
				soundcard_setupaddressspace //
		};
