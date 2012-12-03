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

#define MASKBANKBITS(a) (a & 0x3)
#define MASKCHANBITS(a) (a & 0x7)

void sound_begin() {
	dma_begin();
}

void sound_uploadsamplefromfile() {

}

void sound_uploadsample(uint16_t* data, unsigned short bank, uint16_t bankoffset, uint16_t samplelen) {

	uint16_t* realaddress;
	switch (MASKBANKBITS(bank)) {
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

void sound_configurechannel(unsigned short channel) {

	uint16_t* configregister;
	switch (MASKCHANBITS(channel)) {
		case 0:
			configregister = sound_channel_0_config;
			break;
		case 1:
			configregister = sound_channel_1_config;
			break;
		case 2:
			configregister = sound_channel_2_config;
			break;
		case 3:
			configregister = sound_channel_3_config;
			break;
		case 4:
			configregister = sound_channel_4_config;
			break;
		case 5:
			configregister = sound_channel_5_config;
			break;
		case 6:
			configregister = sound_channel_6_config;
			break;
		case 7:
			configregister = sound_channel_7_config;
			break;
	}

	configregister = 0;
}

void sound_setchannelvolume(unsigned short channel, uint16_t channelvolume) {
	uint16_t* volumeregister;
	switch (MASKCHANBITS(channel)) {
		case 0:
			volumeregister = sound_channel_0_volume;
			break;
		case 1:
			volumeregister = sound_channel_1_volume;
			break;
		case 2:
			volumeregister = sound_channel_2_volume;
			break;
		case 3:
			volumeregister = sound_channel_3_volume;
			break;
		case 4:
			volumeregister = sound_channel_4_volume;
			break;
		case 5:
			volumeregister = sound_channel_5_volume;
			break;
		case 6:
			volumeregister = sound_channel_6_volume;
			break;
		case 7:
			volumeregister = sound_channel_7_volume;
			break;
	}

	volumeregister = channelvolume;
}

void sound_enable() {

}

void sound_disable() {

}

void sound_setmastervolume(uint16_t mastervolume) {
}

void sound_commit() {
	dma_commit();
}
