/*
 * sound.c
 *
 *  Created on: Apr 16, 2012
 *      Author: daniel
 */

#include <stdint.h>
#include <stdbool.h>
#include "include/sound.h"
#include "include/dma.h"
#include "include/sound_registers.h"
#include "include/sound_registermasks.h"

#define MASKBANKBITS(a) (a & 0x3)
#define MASKCHANBITS(a) (a & 0x7)

void sound_begin() {
	dma_begin();
}

void sound_uploadsamplefromfile() {

}

void sound_uploadsample(uint16_t* data, unsigned short bank, uint16_t bankoffset, uint16_t samplelen) {

	volatile uint16_t* realaddress;
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

void sound_configurechannel(unsigned short channel, bool enabled, bool left, bool right) {

//	uint16_t* configregister;
//	switch (MASKCHANBITS(channel)) {
//		case 0:
//			configregister = sound_channel_0_config;
//			break;
//		case 1:
//			configregister = sound_channel_1_config;
//			break;
//		case 2:
//			configregister = sound_channel_2_config;
//			break;
//		case 3:
//			configregister = sound_channel_3_config;
//			break;
//		case 4:
//			configregister = sound_channel_4_config;
//			break;
//		case 5:
//			configregister = sound_channel_5_config;
//			break;
//		case 6:
//			configregister = sound_channel_6_config;
//			break;
//		case 7:
//			configregister = sound_channel_7_config;
//			break;
//	}

	uint16_t config = 0;

	if (left)
		config |= SOUND_CHANNEL_LEFT;

	if (right)
		config |= SOUND_CHANNEL_RIGHT;

	if (enabled)
		config |= SOUND_CHANNEL_ENABLED;

	sound_channel_0_config = config;
}

void sound_setchannelvolume(unsigned short channel, uint16_t channelvolume) {

	switch (MASKCHANBITS(channel)) {
		case 0:
			sound_channel_0_volume = channelvolume;
			break;
		case 1:
			sound_channel_1_volume = channelvolume;
			break;
		case 2:
			sound_channel_2_volume = channelvolume;
			break;
		case 3:
			sound_channel_3_volume = channelvolume;
			break;
		case 4:
			sound_channel_4_volume = channelvolume;
			break;
		case 5:
			sound_channel_5_volume = channelvolume;
			break;
		case 6:
			sound_channel_6_volume = channelvolume;
			break;
		case 7:
			sound_channel_7_volume = channelvolume;
			break;
	}
}

void sound_enable() {

}

void sound_disable() {

}

void sound_setmastervolume(uint16_t mastervolume) {
	sound_channel_master_volume = mastervolume;
}

void sound_configuremasterchannel(bool enabled) {
	sound_channel_master_config = SOUND_CHANNEL_ENABLED;
}

void sound_setchannellength(unsigned short channel, uint16_t length) {
	sound_channel_0_samplelength = length;
}

void sound_commit() {
	dma_commit();
}
