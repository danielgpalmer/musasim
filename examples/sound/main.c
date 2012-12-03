/*
 * main.c
 *
 *  Created on: 22 Sep 2012
 *      Author: daniel
 */

#include <libunagipai/pff.h>
#include <libunagipai/utils.h>
#include <libunagipai/sound.h>

#define SAMPLE_KICK1 "kick1.as2"

int main() {
	FATFS fs;
	FRESULT result;
	result = pf_mount(&fs);
	util_printffresult(result);
	util_printfat(&fs);
	result = pf_open(SAMPLE_KICK1);
	util_printffresult(result);
	uint8_t buf[64];
	uint16_t len;
	pf_read(buf, 63, &len);

	sound_uploadsample(buf, 0, 0, len);

	while (1) {
	};

}
