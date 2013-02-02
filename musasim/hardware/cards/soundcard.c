#define WANTSOUNDFUNC
#include <sound_registermasks.h>
#include "ringbuffer.h"
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
static soundcardchannel channels[TOTALCHANNELS];
static soundcardchannel channelslatched[TOTALCHANNELS];
static uint32_t channelregisterbase;
static uint32_t channelbases[TOTALCHANNELS];
//

// This is a ring buffer for the computed audio
#define OUTPUTCHANNELS 2
#define BUFFER ((4 * RATE) * OUTPUTCHANNELS) // 4s
static ringbuffer* audiobuffer;
//

static cardaddressspace* soundcard_setupaddressspace() {
	cardaddressspace* as = NULL;
	return as;
}

static void soundcard_sdlcallback(void* userdata, Uint8* stream, int len) {
	ringbuffer* buff = (ringbuffer*) userdata;
	if (ringbuffer_isempty(buff)) {
		//log_println(LEVEL_INFO, TAG, "audio buffer empty");
		return;
	}

	len /= sizeof(int16_t);
	unsigned int available = ringbuffer_samplesavailable(buff);
	if (len > available) {
		//log_println(LEVEL_INFO, TAG, "available buffer is smaller than what is needed, want %d have %u", len,
		//		available);
		len = available;
	}
	for (int i = 0; i < len; i++) {
		int16_t value = ringbuffer_get(buff);
		*((int16_t*) stream) = value;
		stream += sizeof(value);
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
			chan->config = 0;
			chan->volume = 0;
			chan->samplepointer = 0;
			chan->samplepos = 0;
			chan->samplelength = 0;
		}
	}
}

static void soundcard_init() {

	sampleram = malloc(SAMPLETOTAL);
	if (sampleram == NULL ) {
		log_println(LEVEL_DEBUG, TAG, "sample ram malloc failed");
	}

	audiobuffer = ringbuffer_new(BUFFER);

	channelregisterbase = utils_nextpow(SAMPLETOTAL);

	log_println(LEVEL_DEBUG, TAG, "registers start at 0x%08x", channelregisterbase);
	soundcard_channelbases(channelbases, channelregisterbase);
	for (int i = 1; i < TOTALCHANNELS; i++) {
		log_println(LEVEL_INFO, TAG, "Channel %d is at 0x%08x", i - 1, channelbases[i]);
	}

	SDL_AudioSpec fmt;
	fmt.freq = RATE;
	fmt.format = AUDIO_S16SYS; // BE samples will get converted to the local format
	fmt.channels = OUTPUTCHANNELS;
	fmt.samples = 512;
	fmt.callback = soundcard_sdlcallback;
	fmt.userdata = audiobuffer;

	if (SDL_OpenAudio(&fmt, NULL ) == 0) {
		SDL_PauseAudio(0);
		log_println(LEVEL_DEBUG, TAG, "SDL output is now active");
		active = true;
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
	ringbuffer_free(audiobuffer);
}

//static int16_t soundcard_mixsamples(int16_t sample1, int16_t sample2, uint8_t volume) {
//
//	float fvolume = volume / UINT8_MAX;
//
//	int32_t mixed = sample1 + (sample2 * fvolume);
//
//// clamp
//	if (mixed > INT16_MAX) {
//		mixed = INT16_MAX;
//	}
//	else if (mixed < INT16_MIN) {
//		mixed = INT16_MIN;
//	}
//
//	return (int16_t) mixed;
//}

static void soundcard_tick(int cyclesexecuted) {

	static int carry = 0;

//if (!active) {
//	return;
//}
	int total = cyclesexecuted + carry;
	carry = total % TICKSPERSAMPLE;
	int ticks = total - carry;
	int samples = ticks / TICKSPERSAMPLE;
	//log_println(LEVEL_INFO, TAG, "executed %d carry %d ticks %d samples %d", cyclesexecuted, carry, ticks, samples);
	if (samples == 0)
		return;

	//SDL_LockAudio();
	for (int i = 0; i < samples * 3; i++) {

		masterchannel* master = &(channels[0].master);

		if (master->config && SOUND_CHANNEL_ENABLED) { // sound is enabled

			int16_t leftSample = 0;
			int16_t rightSample = 0;

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
						leftSample = READ_WORD(sampleram, sampleoffset);
					}

					// RIGHT
					if (chan->config & SOUND_CHANNEL_RIGHT) {
						//rightSample = soundcard_mixsamples(rightSample, READ_WORD(sampleram, sampleoffset),
						//		VOLRIGHT(chan->volume));
						rightSample = READ_WORD(sampleram, sampleoffset);
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

			ringbuffer_put(audiobuffer, leftSample);
			ringbuffer_put(audiobuffer, rightSample);

			// we have caught up with the output
			if (ringbuffer_isfull(audiobuffer)) {
				log_println(LEVEL_INFO, TAG, "audio buffer has overflown");
				break;
			}

		}
	}
	//SDL_UnlockAudio();
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

static const bool soundcard_validaddress(uint32_t address) {
	return true;
}

ringbuffer* sound_getbuffer(unsigned int* head, unsigned int* len) {
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
				NULL, //
				soundcard_setupaddressspace //
		};
