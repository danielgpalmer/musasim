/*
 * basicsound.c
 *
 *  Created on: 22 May 2012
 *      Author: daniel
 */

#include <stdint.h>
#include "basicsound.h"
#include "../../logging.h"
#include "../board.h"
#include "blipbuf/blip_buf.h"

static const char TAG[] = "basicsound";

static uint16_t divisor = 0;
static uint16_t latcheddivisor = 0;
static uint16_t counter = 0;
static bool polarity = false;

static bool basicsound_validaddress(uint32_t address) {
	return true;
}

static void basicsound_tick() {

	counter += 1;
	if (counter >= latcheddivisor) {
		counter = 0;
		latcheddivisor = divisor;
		polarity = !polarity;
	}

}

static void basicsound_write_word(uint32_t address, uint16_t data) {
	divisor = data;
}

static uint16_t basicsound_read_word(uint32_t address) {
	return divisor;
}

const card basicsoundcard = { "BASIC SOUND CARD", NULL, NULL, NULL, basicsound_tick, NULL, NULL,
		basicsound_validaddress, NULL, basicsound_read_word, NULL, NULL, basicsound_write_word, NULL };
