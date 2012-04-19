/*
 * main.c
 *
 *  Created on: Jul 17, 2011
 *      Author: daniel
 */

#include <libunagipai/machine.h>
#include <libunagipai/pff.h>
#include <libunagipai/video_registers.h>
#include <libunagipai/video_registermasks.h>
#include <libunagipai/dma_registers.h>
#include <libunagipai/dma_registermasks.h>
#include <libunagipai/input_registers.h>
#include <libunagipai/sound_registers.h>
#include <libunagipai/lzfx.h>

#include "main.h"

#include <stdio.h>
#include <stdint.h>
#include "fontrom.h"

#include <libunagipai/video.h>

#include "blip.c" // cant be arsed with linking right now
void interrupthandler() {

}

void sound_handler() {

}

void vblank_handler() {

	static uint16_t lastframe = 0;
	static uint16_t thisframe;
	static unsigned int y = 0, x = 10;
	static int xinc = 1, yinc = 1;

	//*video_register_winx = 50;
	//*video_register_winy = 50;
	//*video_register_winwidth = VIDEO_WIDTH - 100;
	//*video_register_winheight = VIDEO_HEIGHT - 100;

	uint16_t vidflags = *video_register_flags;
	uint8_t port0 = *input_start;

	thisframe = *video_register_frame;

	for (int i = 0; i < thisframe - lastframe; i++) {

		static volatile uint16_t* offset;
		offset = video_start + (VIDEO_PLAYFIELDWIDTH * y) + x;
		*(offset) = x * y;

		x += xinc;

		if (x == VIDEO_WIDTH - 1 || x == 0) {
			xinc = -xinc;
		}

		y += yinc;

		if (y == VIDEO_HEIGHT - 1 || y == 0) {
			yinc = -yinc;
		}
	}

	//col = 0;
	//row = 0;
	//gputs("Shizzle me nizzle dizzle bizzle izzle. ABCDEFGHI");

	lastframe = thisframe;
}

void uart_handler() {
}

void initvideo() {
	*video_register_config |= VIDEO_CONFIG_MODE_BITMAP;

	video_clear();

	*video_register_config |= VIDEO_CONFIG_ENVBINT;
}

static void printffresult(FRESULT result) {
	switch (result) {
		case FR_OK:
			printf("OK!\n");
			break;
		case FR_DISK_ERR:
			printf("DISK ERROR!\n");
			break;
		case FR_NOT_READY:
			printf("NOT READY!\n");
			break;
		case FR_NO_FILE:
			printf("NO FILE!\n");
			break;
		case FR_NO_PATH:
			printf("NO PATH\n"); // wtf does no path mean heh
			break;
		case FR_NOT_OPENED:
			printf("FILE NOT OPENED\n");
			break;
		case FR_NOT_ENABLED:
			printf("NOT ENABLED\n");
			break;
		case FR_NO_FILESYSTEM:
			printf("NO FILESYSTEM\n");
			break;
	}
}

static void printfat(FATFS* fs) {
	printf("fat base : 0x%08x\n", fs->fatbase);
	printf("data base : 0x%08x\n", fs->database);
}

void fatimageloader DATALOADERARGS {
	uint16_t len;
	pf_read(buff, wanted, &len);

}

int main(void) {

	uint16_t sr = machine_getstatusregister();
	machine_setstatusregister((sr & 0xf8ff));

	initvideo();

	//*uart_chan0_interruptenable |= INTERRUPTENABLE_ERBFI;

	//uint16_t* blip = _binary_blip_start;

	//for (int i = 0; i < sizeof(_binary_blip_start) / 2; i++) {
	//	*(sound_bank_0 + i) = *blip++;
	//}
	//*sound_channel_0_samplelength = sizeof(_binary_blip_start);
	//*sound_channel_master_config = 0xFFFF;
	//*sound_channel_master_volume = 0xFF99;
	//*sound_channel_0_volume = 0xFF22;
	//*sound_channel_0_config = 0xF9FF;

	FATFS fs;
	FRESULT result;
	result = pf_mount(&fs);
	printffresult(result);
	printfat(&fs);
	result = pf_open("test.txt");
	printffresult(result);
	char buf[64];
	uint16_t len;
	pf_read(buf, 63, &len);

	printf("read from file: %s\n", buf);

	result = pf_open("pai.bes");
	printffresult(result);
	uint16_t imageline[180 * 2];
	pf_read(imageline, 2 * 2, &len);
	//video_blitimage(180, 167, 20, 20, NULL, fatimageloader);

	result = pf_open("pai.bz");

	printffresult(result);
	uint16_t width, height;
	pf_read(&width, 2, &len);
	pf_read(&height, 2, &len);
	unsigned cdatalen = fs.fsize - 4;
	unsigned int rlen = 2 * (width * height);
	printf("compressed image is %u * %u, compressed data is %u bytes\n", (unsigned) width, (unsigned) height, cdatalen);
	uint8_t* compresseddata = malloc(cdatalen);
	uint8_t* rawdata = malloc(rlen);
	pf_read(compresseddata, cdatalen, &len);
	if (lzfx_decompress(compresseddata, cdatalen, rawdata, &rlen) == 0) {
		printf("decompressed!\n");
		free(compresseddata);
		video_blitimage_nocopy(width, height, 20, 20, rawdata);
		free(rawdata);
	}

	video_gputs("Hello World!", _binary_fontrom_start);

	while (1) {
		//printf("Whassup homes\n");

	}

	return 0;

}
