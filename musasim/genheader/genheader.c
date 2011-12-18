/*
 * genheader.c
 *
 *  Created on: Oct 10, 2011
 *      Author: daniel
 */

#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include <stdbool.h>

#include "../sim.h"
#include "../hardware/board.h"
#include "../hardware/cards/videocard.h"
#include "../hardware/cards/uartcard.h"

char headers[] = "#include <stdint.h>\n"
		"\n\n";

void common();
void machine();
void video();
void sound();
void input();
void uart();

int main(int argc, char* argv[]) {

	if (argc != 2) {
		printf("usage; ./%s [machine|video|sound]\n", argv[0]);
	}
	else {
		if (strcmp(argv[1], "machine") == 0) {
			printf("/* machine.h */\n\n");
			common();
			machine();
			printf("\n");
		}
		else if (strcmp(argv[1], "video") == 0) {
			printf("/* video.h */\n\n");
			common();
			video();
			printf("\n");
		}
		else if (strcmp(argv[1], "sound") == 0) {
			printf("/* sound.h */\n\n");
			common();
			printf("\n");
		}
		else if (strcmp(argv[1], "input") == 0) {
			printf("/* input.h */\n\n");
			common();
			input();
			printf("\n");
		}
		else if (strcmp(argv[1], "uart") == 0) {
			printf("/* uart.h */\n\n");
			common();
			uart();
			printf("\n");
		}
	}
	return 0;
}

void common() {
	printf("%s", headers);
}

void machine() {
	//printf("volatile uint16_t* machine_ram_start = (uint16_t*) 0x%x;\n", OFFSET_RAM);
	//printf("volatile uint16_t* machine_ram_end = (uint16_t*) 0x%x;\n", MAX_RAM);
	//printf("uint16_t machine_ram_size = (uint16_t) 0x%x;\n", SIZE_RAM);
}

void video() {

	uint32_t registers = VIDEO_MEMORYEND;
	registers = video_fillbits(registers);

	printf("volatile uint16_t* video_start = (uint16_t*) 0x%x;\n", SLOT_OFFSET(SLOT_VIDEOCARD));
	printf("volatile uint16_t* video_end = (uint16_t*) 0x%x;\n", SLOT_OFFSET(SLOT_VIDEOCARD) + VIDEO_MEMORYEND);
	printf("volatile uint16_t* video_registers = (uint16_t*) 0x%x;\n", SLOT_OFFSET(SLOT_VIDEOCARD) + registers);
}

void sound() {

}

void uart() {
	printf("volatile uint8_t* uart_chan0_rxtx = (uint8_t*) 0x%x;\n",
			SLOT_OFFSET(SLOT_UARTCARD) + UART_REGISTER_RXTXBUFFER);
	printf("volatile uint8_t* uart_chan0_interruptenable = (uint8_t*) 0x%x;\n",
			SLOT_OFFSET(SLOT_UARTCARD) + UART_REGISTER_INTERRUPTENABLE);
	printf("volatile uint8_t* uart_chan0_linecontrol = (uint8_t*) 0x%x;\n",
			SLOT_OFFSET(SLOT_UARTCARD) + UART_REGISTER_LINECONTROL);
	printf("volatile uint8_t* uart_chan0_linestatus = (uint8_t*) 0x%x;\n",
			SLOT_OFFSET(SLOT_UARTCARD) + UART_REGISTER_LINESTATUS);

}

void input() {
	printf("volatile uint8_t* input_start = (uint8_t*) 0x%x;\n", SLOT_OFFSET(SLOT_INPUTCARD));
}
