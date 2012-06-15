/*
 * main.c
 *
 *  Created on: Jul 17, 2011
 *      Author: daniel
 */

#include <stdio.h>
#include <stdint.h>
#include <inttypes.h>
#include <libunagipai/machine.h>
#include <libunagipai/vt100.h>
#include <libunagipai/uart.h>
#include <libunagipai/input_registers.h>
#include <libunagipai/input_registermasks.h>
#include <libunagipai/dma_registers.h>

void uart_handler() __attribute__ (( interrupt));
void uart_handler() {
	printf("uart int\n");
}

void inthandler1() __attribute__ (( interrupt));
void inthandler1() {
	//printf("int 1\n");
	volatile uint16_t* basicvideo = (uint16_t*) 0x200000;
	static uint8_t i = 0;
	for (int y = 0; y < 128; y++) {
		for (int x = 0; x < (256 / 2); x++) {
			*(basicvideo + (((256 / 2) * y) + x)) = (uint16_t) i | (uint16_t)(i << 8);
		}
	}
	i++;
}

void inthandler3() __attribute__ (( interrupt));
void inthandler3() {
	printf("int 3\n");
}

void inthandler4() __attribute__ (( interrupt));
void inthandler4() {
	printf("int 4\n");
}

void inthandler5() __attribute__ (( interrupt));
void inthandler5() {
	printf("int 5\n");
}

void inthandler6() __attribute__ (( interrupt));
void inthandler6() {
	printf("int 6\n");
}

void inthandler7() __attribute__ (( interrupt));
void inthandler7() {
	printf("int 7\n");
}

static void test_dma() {
	volatile uint16_t* dmabase = (volatile uint16_t*) 0xa00000;
	volatile uint16_t* regwindow = (volatile uint16_t*) 0xa0001E;
	printf("Running DMA tests\n");

	for (int window = 0; window < 16; window++) {
		uint16_t testdata = 0xAA00;
		*regwindow = window;
		for (int reg = 0; reg < 11; reg++) {
			uint16_t data = testdata + window;
			*(dmabase + reg) = data;
			printf ("Window: %d Reg %d: Data Written - 0x%04"PRIx8 "\n", window, reg, data);
			testdata = ~testdata & 0xFF00;
		}
	}

	for (int window = 0; window < 16; window++) {
		uint16_t testdata = 0xAA00;
		*regwindow = window;
		for (int reg = 0; reg < 11; reg++) {
			uint16_t data = testdata + window;
			printf ("Window: %d Reg %d: Data Read - 0x%04"PRIx8"\n", window, reg, *(dmabase + reg));
			testdata = ~testdata & 0xFF00;
		}
	}
}

int main(void) {

	//uart_configureinterrupts(0, true, false, false, false);

	volatile uint16_t* basicvideo = (uint16_t*) 0x200000;
	volatile uint16_t* basicsound = (uint16_t*) 0x600000;

	vt100_resetdevice();
	vt100_setattributes();
	vt100_erase_screen();
	vt100_cursor_home_pos(1, 0);

	test_dma();

	//while(1){};

	//while (1) {

	//	printf("Hello, World! 0x%02"PRIx8"\n", input_port0);

	//	for (int y = 0; y < 128; y++) {
	//		for (int x = 0; x < (256 / 2); x++) {
	//			*(basicvideo + (((256 / 2) * y) + x)) = i;
	//if (y < 128 / 2) {
	//	*(basicvideo + (((256 / 2) * y) + x)) = x < (128 / 2) ? 0xE0E0 : 0x0707;
	//}
	//else {
	//	*(basicvideo + (((256 / 2) * y) + x)) = x < (128 / 2) ? 0x1818 : 0x1f1f;
	//}
	//		}
	//	}
	//	i++;
	//}

	uint16_t sr = machine_getstatusregister();
	machine_setstatusregister((sr & 0xf8ff));

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

	}

	return 0;
}
