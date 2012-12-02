/*
 * sound.c
 *
 *  Created on: Apr 16, 2012
 *      Author: daniel
 */

#include <stdint.h>
#include "include/sound.h"
#include "include/dma.h"

void sound_start() {
	dma_start();
}

void sound_uploadsamplefromfile() {

}

void sound_uploadsample(uint16_t* data, unsigned int bank, uint16_t bankoffset) {

}

void sound_commit() {
	dma_commit();
}
