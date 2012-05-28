/*
 * basicsound.c
 *
 *  Created on: 22 May 2012
 *      Author: daniel
 */

#include "basicsound.h"
#include "../../logging.h"
#include "../board.h"

static const char TAG[] = "basicsound";

static void basicsound_init() {

}

static void basicsound_dispose() {
}

static bool basicsound_validaddress(uint32_t address) {
	return true;
}

static void basicsound_tick() {

}

static void basicsound_write_word(uint32_t address, uint16_t data) {

}

static uint16_t basicsound_read_word(uint32_t address) {
	return 0;
}

static void basicsound_irqack() {
	board_lower_interrupt(&basicsoundcard);
}

const card basicsoundcard = { "BASIC SOUND CARD", basicsound_init, basicsound_dispose, NULL, basicsound_tick,
		basicsound_irqack, NULL, basicsound_validaddress, NULL, basicsound_read_word, NULL, NULL, basicsound_write_word,
		NULL };
