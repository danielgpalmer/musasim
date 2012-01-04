/*
 * soundcard.h
 *
 *  Created on: Nov 28, 2011
 *      Author: daniel
 */

#ifndef SOUNDCARD_H_
#define SOUNDCARD_H_

#include "card.h"

const card soundcard;

#define RATE 22050
#define TICKSPERSAMPLE (SIM_TICKS_PERSECOND / RATE)

#define NUMAUDIOCHANNELS 8
#define TOTALCHANNELS (NUMAUDIOCHANNELS + 1)

#define SAMPLETOTAL (SAMPLEPAGESIZE * SAMPLEPAGES)

#define SAMPLEPAGES 4
#define SAMPLEPAGESIZE 0xFFFF

/*
 *	15	10	09	08	07	06	05	04	03	02	01	00
 *	i	[ P	]	[	v	]	[R	]	l	r	I	E
 *
 *	E - Enabled
 *	I - Interrupt
 *	r - Right Side
 *	l - Left Side
 *	R - Rate, divisor of the master clock
 *	P - Page
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
	uint16_t volume;

} audiochannel;

#define SOUND_REGISTER_CONFIG 0
#define SOUND_REGISTER_SAMPLEPOINTER 2
#define SOUND_REGISTER_SAMPLELENGTH 4
#define SOUND_REGISTER_SAMPLEPOS 6
#define SOUND_REGISTER_VOLUME 8

typedef union {
	masterchannel master;
	audiochannel audio;
} channel;

#ifdef WANTSOUNDFUNC
#include "../../utils.h"
#include <assert.h>
static void soundcard_channelbases(uint32_t* channelbases, uint32_t channelregisterbase) {
	int registerspaddedsize = utils_nextpow(sizeof(channel));
	assert(registerspaddedsize != 0);
	for (int i = 0; i < TOTALCHANNELS; i++) {
		channelbases[i] = channelregisterbase + (registerspaddedsize * i);
	}
}
#endif

#endif /* SOUNDCARD_H_ */
