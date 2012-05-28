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

static const char TAG[] = "basicsound";

static uint16_t divisor = 0;

static bool basicsound_validaddress(uint32_t address) {
	return true;
}

static void basicsound_tick() {

}

static void basicsound_write_word(uint32_t address, uint16_t data) {
	divisor = data;
}

static uint16_t basicsound_read_word(uint32_t address) {
	return divisor;
}

const card basicsoundcard = { "BASIC SOUND CARD", NULL, NULL, NULL, basicsound_tick, NULL, NULL,
		basicsound_validaddress, NULL, basicsound_read_word, NULL, NULL, basicsound_write_word, NULL };
