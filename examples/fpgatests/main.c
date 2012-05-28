/*
 * main.c
 *
 *  Created on: Jul 17, 2011
 *      Author: daniel
 */

#include <stdio.h>
#include <stdint.h>
#include <inttypes.h>
#include <libunagipai/vt100.h>
#include <libunagipai/input_registers.h>
#include <libunagipai/input_registermasks.h>

int main(void) {

	volatile uint16_t* basicvideo = (uint16_t*) 0x200000;
	volatile uint16_t* basicsound = (uint16_t*) 0x600000;

	vt100_resetdevice();
	vt100_setattributes();
	vt100_erase_screen();
	vt100_cursor_home_pos(1, 0);

	printf("Hello, World!\n");
	uint16_t c = 0;

	for (int y = 0; y < 128; y++) {
		for (int x = 0; x < (256 / 2); x++) {
			if (y > 128 / 2) {
				*(basicvideo + (((256 / 2) * y) + x)) = x < (128 / 2) ? 0xE0E0 : 0x0707;
			}
			else {
				*(basicvideo + (((256 / 2) * y) + x)) = x < (128 / 2) ? 0x1818 : 0x1f1f;
			}
		}
	}

	while (1) {
		vt100_cursor_save;
		vt100_erase_line();
		uint8_t port0 = input_port0;
		printf("pwm divisor 0x%04"PRIx16" ", *basicsound);
		if (port0 != 0xFF) {
			printf("port0 0x%02x - ", port0);
			if ((port0 & ~BUTTON_UP) == 0) {
				printf("UP ");
				*basicsound += 10;
			}
			if ((port0 & ~BUTTON_DOWN) == 0) {
				printf("DOWN ");
				*basicsound -= 10;
			}
			if ((port0 & ~BUTTON_LEFT) == 0) {
				printf("LEFT ");
				*basicsound -= 1;
			}
			if ((port0 & ~BUTTON_RIGHT) == 0) {
				printf("RIGHT ");
				*basicsound += 1;
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
		}
		printf("\n");
		vt100_cursor_unsave();

		//for (int y = 0; y < 128; y++) {
		//	for (int x = 0; x < (256 / 2); x++) {
		//		*(basicvideo + (((256 / 2) * y) + x)) = x * y;
		//	}
		//}
		c++;

	}
	return 0;
}
