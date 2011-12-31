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

static char TAG[] = "sound";

#define NUMCHANNELS 8
#define SAMPLEPAGES 4
#define SAMPLEPAGESIZE 0xFFFF
#define SAMPLETOTAL (SAMPLEPAGESIZE * SAMPLEPAGES)

typedef struct {
	uint16_t config;
	uint16_t samplepointer;
	uint16_t samplelength;
	uint16_t samplepos;

} channel;

static uint16_t config;
static int16_t* sampleram;
static channel channels[NUMCHANNELS];

static uint32_t registerbase;
static uint32_t channelbases[NUMCHANNELS];
static uint32_t configaddress;

static void soundcard_init() {

	sampleram = malloc(SAMPLETOTAL);

	for (int i = 0; i < NUMCHANNELS; i++) {
		channel* chan = &(channels[i]);
		chan->samplepointer = 0;
		chan->samplelength = 0;
	}

	registerbase = utils_nextpow(SAMPLETOTAL);

	log_println(LEVEL_DEBUG, TAG, "registers start at 0x%08x", registerbase);

	int registerspaddedsize = utils_nextpow(sizeof(channel));
	for (int i = 0; i < NUMCHANNELS; i++) {
		channelbases[i] = registerbase + (registerspaddedsize * i);
		log_println(LEVEL_DEBUG, TAG, "channel %d base is at 0x%08x ", i, channelbases[i]);
	}

	configaddress = utils_nextpow(channelbases[NUMCHANNELS - 1] + sizeof(channel));

	log_println(LEVEL_DEBUG, TAG, "config register is at 0x%08x", configaddress);

}

static void soundcard_dispose() {
	free(sampleram);
}

static void soundcard_tick() {

	for (int i = 0; i < NUMCHANNELS; i++) {
		channel* chan = &(channels[i]);
		chan->samplepos++;
	}

}

static void* soundcard_decodereg(uint32_t address) {

	return NULL;

}

static uint16_t soundcard_read_word(uint32_t address) {

	soundcard_decodereg(address);
	return 0;
}

static void soundcard_write_word(uint32_t address, uint16_t value) {

	soundcard_decodereg(address);
	config = value;

}

const card soundcard = { "SOUND CARD", soundcard_init, soundcard_dispose, soundcard_tick, NULL, NULL, NULL,
		soundcard_read_word, NULL, NULL, soundcard_write_word, NULL };
