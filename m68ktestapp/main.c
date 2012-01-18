/*
 * main.c
 *
 *  Created on: Jul 17, 2011
 *      Author: daniel
 */

#include <stdio.h>
#include <stdint.h>
#include "fontrom/fontrom.h"
#include "../musasim/genheader/machine.h"
#include "../musasim/genheader/video.h"
#include "../musasim/genheader/input.h"
#include "../musasim/genheader/uart.h"
#include "../musasim/genheader/sound.h"

#include "blip.c" // cant be arsed with linking right now
#define PIXELSIZE 2
#define WIDTH 480
#define HEIGHT 272

int16_t initedvar = 0;

//volatile char* magic = (volatile char*) 0x200000;
//volatile uint16_t* video = (volatile uint16_t*) 0x300000;

char astring[] = "this is a string";
volatile char something[4] = { 0xff, 0xaa, 0xff, 0xaa };

void sputch(char ch) {
	while ((*(uart_chan0_linestatus) & LINESTATUS_TRANSMITTERHOLDINGREGISTEREMPTY)
			!= LINESTATUS_TRANSMITTERHOLDINGREGISTEREMPTY) {
		// nop
	}
	*uart_chan0_rxtx = ch;
}


char sgetch() {

	while (!(*(uart_chan0_linestatus) & LINESTATUS_DATAREADY)) {

	}

	return *uart_chan0_rxtx;
}

//void puts(char* string) {
//
//	char c;
//
//	while ((c = *string++) != 0) {
//		*magic = c;
//	}
//}

void interrupthandler() __attribute__ (( interrupt ));
void interrupthandler() {

}

void sound_handler() __attribute__ ((interrupt));
void sound_handler() {

}

void vblank_handler() __attribute__ (( interrupt));
void vblank_handler() {

	static uint16_t lastframe = 0;
	static uint16_t thisframe;
	static unsigned int y = 0, x = 0;
	static int xinc = 1, yinc = 1;

	uint16_t vidflags = *video_register_flags;
	uint8_t port0 = *input_start;

	thisframe = *video_register_frame;

	for (int i = 0; i < thisframe - lastframe; i++) {

		*(video_start + (WIDTH * y) + x) = x * y;

		x += xinc;

		if (x == WIDTH - 1 || x == 0) {
			xinc = -xinc;
		}

		y += yinc;

		if (y == HEIGHT - 1 || y == 0) {
			yinc = -yinc;
		}

	}

	lastframe = thisframe;
}

void uart_handler() __attribute__ (( interrupt));
void uart_handler() {
	//sputch(sgetch());
}

uint16_t getstatusregister() {
	uint16_t temp;
	asm volatile (
			"move.w %%sr, %0\n\t"
			:
			: "g"(temp)
			: );
	return temp;
}

void setstatusregister(uint16_t value) {
	asm volatile (
			"move.w %0, %%sr\n\t"
			:
			: "g"(value)
			: );
}

int col = 0;

void gputs(char* string) {

	int charoffset = 33 * 16;
	char c;

	while ((c = *string++) != 0) {
		for (int i = 0; i < 16; i++) {

			uint8_t character = _binary_fontrom_start[i + ((c * 16) - charoffset)];

			for (int j = 0; j < 8; j++) {

				int pixel = character & 0x01;
				if (pixel) {
					*(video_start + (WIDTH * i) + j + (col * 8)) = 0x000000;
				}
				else {
					*(video_start + (WIDTH * i) + j + (col * 8)) = 0xFFFFFF;
				}
				character = (character >> 1);
			}
		}

		col++;
	}
}

void initvideo() {
	*video_register_config |= VIDEO_CONFIG_MODE_BITMAP;
	*dma_register_counth = 0x0001;
	*dma_register_countl = 0xFE00;
	*dma_register_desth = 0x0020;
	*dma_register_datal = 0xFFFF;
	//*dma_register_config |= DMA_REGISTER_CONFIG_START | DMA_REGISTER_CONFIG_SIZE | DMA_REGISTER_CONFIG_MODE
	//		| DMA_REGISTER_CONFIG_DATAACT_INVERSE | DMA_REGISTER_CONFIG_DSTACT_INCTWO;

	*dma_register_config = DMA_REGISTER_CONFIG_START | DMA_REGISTER_CONFIG_MODE_FILL | DMA_REGISTER_CONFIG_SIZE
			| DMA_REGISTER_CONFIG_DATAACT_INVERSE | DMA_REGISTER_CONFIG_DSTACT_INCTWO;

	while (!(*dma_register_config & DMA_REGISTER_CONFIG_DONE)) {

	}

	//for (int y = 0; y < HEIGHT; y++) {
	//	for (int x = 0; x < WIDTH; x++) {
	//		*(video_start + (WIDTH * y) + x) = 0xffff;
	//	}
	//}
	*video_register_config |= VIDEO_CONFIG_ENVBINT;
}

int main(void) {

	//uint16_t sr = getstatusregister();
	//setstatusregister((sr & 0xf8ff));

//char helloworld[] = "Hello, World!\n";
//char* string = helloworld;

//puts(helloworld);

	//initvideo();

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

		printf("Whassup homes\n");
		//gputs("Hello World!");

	}

//*(video + (480 * 240) + 100) = 0xFFFF;
	initedvar = 0;
	return 0;

}
