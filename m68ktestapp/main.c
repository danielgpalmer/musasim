/*
 * main.c
 *
 *  Created on: Jul 17, 2011
 *      Author: daniel
 */

#include <stdint.h>
#include "fontrom/fontrom.h"

#define PIXELSIZE 2
#define WIDTH 480
#define HEIGHT 272

int16_t initedvar = 0;

volatile char* magic = (volatile char*) 0x200000;

volatile char something[4] = { 0xff, 0xaa, 0xff, 0xaa };

void puts(char* string) {

	char c;

	while ((c = *string++) != 0) {
		*magic = c;
	}
}

int main(void) {

	volatile uint16_t* video = (volatile uint16_t*) 0x300000;

	char helloworld[] = "Hello, World!\n";
	char* string = helloworld;

	puts(helloworld);

	while (1) {

		//for (int y = 0; y < HEIGHT; y++) {
		//	for (int x = 0; x < WIDTH; x++) {
		//		*(video + (WIDTH * y) + x) = x * y;
		//	}
		//}

		for (int i = 0; i < 16; i++) {

			uint8_t character = _binary_fontrom_start[i];

			for (int j = 0; j < 8; j++) {

				int pixel = character & 0x01;
				if (pixel) {
					*(video + (WIDTH * i) + j) = 0x000000;
				}
				else {
					*(video + (WIDTH * i) + j) = 0xFFFFFF;
				}
				character = (character >> 1);
			}
		}
	}

//*(video + (480 * 240) + 100) = 0xFFFF;
	initedvar = 0;
	return 0;

}
