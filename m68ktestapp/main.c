/*
 * main.c
 *
 *  Created on: Jul 17, 2011
 *      Author: daniel
 */

#include <stdint.h>

#define PIXELSIZE 2
#define WIDTH 480
#define HEIGHT 272

int16_t initedvar = 0;

volatile char* magic = (volatile char*) 0x200000;

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


		for (int y = 0; y < HEIGHT; y++) {
			for (int x = 0; x < WIDTH; x++) {
				*(video + (WIDTH * y) + x) = x * y;
			}
		}

		//*(video + (480 * 240) + 100) = 0xFFFF;
		initedvar = 0;
		puts("frame\n");
	}

	return 0;

}
