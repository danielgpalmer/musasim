/*
 * main.c
 *
 *  Created on: Jul 17, 2011
 *      Author: daniel
 */

#include <libunagipai/machine.h>
#include <libunagipai/ata.h>
#include <libunagipai/video_registers.h>
#include <libunagipai/video_registermasks.h>
#include <libunagipai/dma_registers.h>
#include <libunagipai/dma_registermasks.h>
#include <libunagipai/input_registers.h>
#include <libunagipai/sound_registers.h>

#include "main.h"

#include <stdio.h>
#include <stdint.h>
#include "fontrom.h"

#include "blip.c" // cant be arsed with linking right now
void interrupthandler() {

}

void sound_handler() {

}

static int col = 0;
static int row = 0;

#define CHARWIDTH 8
#define CHARHEIGHT 16
#define COLS (VIDEO_PLAYFIELDWIDTH/CHARWIDTH)
#define ROWSIZE (COLS * CHARWIDTH * CHARHEIGHT)

void gputs(char* string) {

	int charoffset = 32 * CHARHEIGHT;
	char c = 0;

	while ((c = *string++) != 0) {
		for (int i = 0; i < CHARHEIGHT; i++) {

			uint8_t character = _binary_fontrom_start[i + ((c * CHARHEIGHT) - charoffset)];

			for (int j = 0; j < CHARWIDTH; j++) {

				int offset = (ROWSIZE * row) + ((VIDEO_PLAYFIELDWIDTH * i) + j + (col * CHARWIDTH));

				int pixel = character & 0x01;
				if (pixel) {
					*(video_start + offset) = 0x0000;
				}
				else {
					*(video_start + offset) = 0xFFFF;
				}
				character = (character >> 1);
			}
		}

		col++;
	}
}

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

		static volatile uint16_t* offset;
		offset = video_start + (VIDEO_PLAYFIELDWIDTH * y) + x;
		*(offset) = x * y;

		x += xinc;

		if (x == VIDEO_PLAYFIELDWIDTH - 1 || x == 0) {
			xinc = -xinc;
		}

		y += yinc;

		if (y == VIDEO_PLAYFIELDHEIGHT - 1 || y == 0) {
			yinc = -yinc;
		}
	}

	col = 0;
	row = 0;
	//gputs("Shizzle me nizzle dizzle bizzle izzle. ABCDEFGHI");

	lastframe = thisframe;
}

void uart_handler() {
}

void initvideo() {
	*video_register_config |= VIDEO_CONFIG_MODE_BITMAP;

	static uint32_t count = VIDEO_PLAYFIELDWIDTH * VIDEO_PLAYFIELDHEIGHT;

	*dma_register_counth = (count >> 16) & 0xFFFF;
	*dma_register_countl = (count & 0xFFFF);
	*dma_register_desth = 0x0020;
	*dma_register_destl = 0x0000;
	*dma_register_datal = 0xFFFF;
	//*dma_register_config |= DMA_REGISTER_CONFIG_START | DMA_REGISTER_CONFIG_SIZE | DMA_REGISTER_CONFIG_MODE
	//		| DMA_REGISTER_CONFIG_DATAACT_INVERSE | DMA_REGISTER_CONFIG_DSTACT_INCTWO;

	*dma_register_config = DMA_REGISTER_CONFIG_START | DMA_REGISTER_CONFIG_MODE_FILL | DMA_REGISTER_CONFIG_SIZE
			| DMA_REGISTER_CONFIG_DATAACT_INVERSE | DMA_REGISTER_CONFIG_DSTACT_INCTWO;

	while (!(*dma_register_config & DMA_REGISTER_CONFIG_DONE)) {

	}

	*video_register_config |= VIDEO_CONFIG_ENVBINT;
}

int main(void) {

	ata_id id;

	uint16_t sr = machine_getstatusregister();
	machine_setstatusregister((sr & 0xf8ff));

	initvideo();

	//*uart_chan0_interruptenable |= INTERRUPTENABLE_ERBFI;
	//*ide_register_command = ATA_IDENTIFYDRIVE;

	//uint16_t* blip = _binary_blip_start;

	//for (int i = 0; i < sizeof(_binary_blip_start) / 2; i++) {
	//	*(sound_bank_0 + i) = *blip++;
	//}
	//*sound_channel_0_samplelength = sizeof(_binary_blip_start);
	//*sound_channel_master_config = 0xFFFF;
	//*sound_channel_master_volume = 0xFF99;
	//*sound_channel_0_volume = 0xFF22;
	//*sound_channel_0_config = 0xF9FF;

	while (1) {
		//printf("Whassup homes\n");
		//gputs("Hello World!");
		ata_identify(&id);
		printf("model: [%s]\n", id.model);
		printf("drive serial: [%s]\n", id.serial);
		printf("fwrev: [%s]\n", id.fwrev);
		ata_read_sector();
	}

	return 0;

}
