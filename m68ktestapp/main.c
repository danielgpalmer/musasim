/*
 * main.c
 *
 *  Created on: Jul 17, 2011
 *      Author: daniel
 */

#include <stdint.h>
#include "fontrom/fontrom.h"
#include "../musasim/genheader/video.h"

#define PIXELSIZE 2
#define WIDTH 480
#define HEIGHT 272

int16_t initedvar = 0;

//volatile char* magic = (volatile char*) 0x200000;
//volatile uint16_t* video = (volatile uint16_t*) 0x300000;

volatile char something[4] = { 0xff, 0xaa, 0xff, 0xaa };

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

int main(void) {

	uint16_t sr = getstatusregister();

//char helloworld[] = "Hello, World!\n";
//char* string = helloworld;

//puts(helloworld);

	gputs("Hello, world!");

	while (1) {

		//for (int y = 0; y < HEIGHT; y++) {
		//	for (int x = 0; x < WIDTH; x++) {
		//		*(video + (WIDTH * y) + x) = x * y;
		//	}
		//}

	}

//*(video + (480 * 240) + 100) = 0xFFFF;
	initedvar = 0;
	return 0;

}
