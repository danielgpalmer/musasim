/*
 * main.c
 *
 *  Created on: Jul 17, 2011
 *      Author: daniel
 */

#include <stdio.h>
#include <stdint.h>
#include <libunagipai/vt100.h>
#include <libunagipai/input_registers.h>
#include <libunagipai/input_registermasks.h>

int main(void) {

	volatile uint16_t* basicvideo = (uint16_t*) 0x200000;
	vt100_resetdevice();
	vt100_setattributes();
	vt100_erase_screen();
	printf("Hello, World!\n");
	vt100_cursor_home_pos(1, 0);

	for (int y = 0; y < 128; y++) {
		for (int x = 0; x < (256 / 2); x++) {
			*(basicvideo + (((256 / 2) * y) + x)) = 0xFF;
		}
	}

	while (1) {
		vt100_cursor_save;
		vt100_erase_line();
		uint8_t port0 = input_port0;
		if (port0 != 0xFF) {
			printf("port0 0x%02x - ", port0);
			if ((port0 & ~BUTTON_UP) == 0) {
				printf("UP ");
			}
			if ((port0 & ~BUTTON_DOWN) == 0) {
				printf("DOWN ");
			}
			if ((port0 & ~BUTTON_LEFT) == 0) {
				printf("LEFT ");
			}
			if ((port0 & ~BUTTON_RIGHT) == 0) {
				printf("RIGHT ");
			}
			if ((port0 & ~BUTTON_START) == 0) {
				printf("START ");
			}
			if ((port0 & ~BUTTON_A) == 0) {
				printf("A ");
			}
			if ((port0 & ~BUTTON_B) == 0) {
				printf("B ");
			}
			if ((port0 & ~BUTTON_C) == 0) {
				printf("C ");
			}
			printf("\n");
		}
		vt100_cursor_unsave();
	}
	return 0;
}
