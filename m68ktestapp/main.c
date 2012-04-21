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
#include <libunagipai/image.h>

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

static image ball;
static image pai;

void vblank_handler() {

	static uint16_t lastframe = 0;
	static uint16_t thisframe;
	static unsigned int y = 0, x = 0;
	static int xinc = 1, yinc = 1;

	//*video_register_winx = 50;
	//*video_register_winy = 50;
	//*video_register_winwidth = VIDEO_WIDTH - 100;
	//*video_register_winheight = VIDEO_HEIGHT - 100;

	uint16_t vidflags = *video_register_flags;
	uint8_t port0 = *input_start;

	thisframe = *video_register_frame;

	for (int i = 0; i < thisframe - lastframe; i++) {

		x += xinc;

		if (x == VIDEO_WIDTH - ball.width - 1 || x == 0) {
			xinc = -xinc;
		}

		y += yinc;

		if (y == VIDEO_HEIGHT - ball.height - 1 || y == 0) {
			yinc = -yinc;
		}
	}

	video_begin();
	video_clear();
	video_blitimage_nocopy(pai.width, pai.height, 30, 30, pai.data);
	video_blitimage_nocopy(ball.width, ball.height, x, y, ball.data);
	video_commit();

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

	image_loadimagefromfile(&fs, &pai, "pai.bz", true);
	image_loadimagefromfile(&fs, &ball, "ball.be", false);
	initvideo();

	video_gputs("Hello World!", _binary_fontrom_start);

	while (1) {
		//printf("Whassup homes\n");

	}

	return 0;

}
