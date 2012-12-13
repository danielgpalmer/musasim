/*
 * main.c
 *
 *  Created on: 22 Sep 2012
 *      Author: daniel
 */

#include <libunagipai/pff.h>
#include <libunagipai/utils.h>
#include <libunagipai/sound.h>

#include <stdlib.h>
#include <stdbool.h>

#define SAMPLE_SINE "sine.as2"
#define SAMPLE_KICK1 "kick1.as2"

int main() {
	FATFS fs;
	FRESULT result;
	result = pf_mount(&fs);
	util_printffresult(result);
	util_printfat(&fs);
	result = pf_open(SAMPLE_KICK1);
	util_printffresult(result);

	uint8_t* buf = malloc(9664);
	uint16_t len;
	pf_read(buf, 9664, &len);

	sound_uploadsample(buf, 0, 0, len);
	sound_setmastervolume(0xffff);
	sound_configurechannel(0, true, true, true);
	sound_setchannellength(0, len / 2);
	sound_setchannelvolume(0, 0xffff);
	sound_configuremasterchannel(true);

	while (1) {
	};

}
