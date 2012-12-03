/*
 * sound.c
 *
 *  Created on: Apr 16, 2012
 *      Author: daniel
 */

#include <stdint.h>
#include "include/sound.h"
#include "include/dma.h"
#include "include/sound_registers.h"

void sound_start() {
	dma_start();
}

void sound_uploadsamplefromfile() {

}

void sound_uploadsample(uint16_t* data, unsigned int bank, uint16_t bankoffset, uint16_t samplelen) {

	uint16_t* realaddress;
	switch (bank & 0x3) {
		case 0:
			realaddress = sound_bank_0 + bankoffset;
			break;
		case 1:
			realaddress = sound_bank_1 + bankoffset;
			break;
		case 2:
			realaddress = sound_bank_2 + bankoffset;
			break;
		case 3:
			realaddress = sound_bank_3 + bankoffset;
			break;
	}

	for (int i = 0; i < samplelen / 2; i++) {
		*realaddress++ = data++;
	}

}

void sound_commit() {
	dma_commit();
}
