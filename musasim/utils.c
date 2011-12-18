/*
 * utils.c
 *
 *  Created on: Dec 18, 2011
 *      Author: daniel
 */

#include "utils.h"

uint32_t utils_nextpow(uint32_t value) {
	value--;
	value |= value >> 1;
	value |= value >> 2;
	value |= value >> 4;
	value |= value >> 8;
	value |= value >> 16;
	value++;
	return value;
}
