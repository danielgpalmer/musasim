/*
 * inputcard.c
 *
 *  Created on: Dec 3, 2011
 *      Author: daniel
 */

#include "inputcard.h"

uint8_t inputcard_read_byte() {
	return 0;
}

card inputcard = { "INPUT CARD", NULL, NULL, NULL, inputcard_read_byte, NULL, NULL, NULL, NULL, NULL };
