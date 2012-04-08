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
#include "../hardware/cards/dmacard.h"
#define WANTSOUNDFUNC
#include "../hardware/cards/soundcard.h"
#include "../utils.h"

char headers[] = "#include <stdint.h>\n"
		"\n\n";

#define SIZEOFARRAY(A) (sizeof(A)/sizeof(A[0]))

static void common(void);
static void dma(void);
static void ata(void);
static void video(void);
static void sound(void);
static void input(void);
static void uart(void);

#define PRE(tag) printf("#ifndef _LIBUNAGIPAI_%s\n#define _LIBUNAGIPAI_%s\n\n\n", tag, tag)
#define POST(tag) printf("#endif\n")

int main(int argc, char* argv[]) {

	if (argc != 2) {
		printf("usage; ./%s [machine|video|sound]\n", argv[0]);
	}
	else {
		if (strcmp(argv[1], "dma") == 0) {
			printf("/* dma.h */\n\n");
			PRE("DMA");
			common();
			dma();
			POST("DMA");
		}
		if (strcmp(argv[1], "ata") == 0) {
			printf("/* ata.h */\n\n");
			PRE("ATA");
			common();
			ata();
			POST("ATA");
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
			sound();
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

void dma() {
	//printf("volatile uint16_t* machine_ram_start = (uint16_t*) 0x%x;\n", OFFSET_RAM);
	//printf("volatile uint16_t* machine_ram_end = (uint16_t*) 0x%x;\n", MAX_RAM);
	//printf("uint16_t machine_ram_size = (uint16_t) 0x%x;\n", SIZE_RAM);

	char* dmaregisternames[] = { "dma_register_config", "dma_register_datah", "dma_register_datal",
			"dma_register_counth", "dma_register_countl", "dma_register_srch", "dma_register_srcl",
			"dma_register_desth", "dma_register_destl" };
	uint32_t dmaregisteraddresses[] = { SLOT_OFFSET(SLOT_DMACARD) + DMACARD_REGISTER_CONFIG, SLOT_OFFSET(SLOT_DMACARD)
			+ DMACARD_REGISTER_DATAH, SLOT_OFFSET(SLOT_DMACARD) + DMACARD_REGISTER_DATAL, SLOT_OFFSET(SLOT_DMACARD)
			+ DMACARD_REGISTER_COUNTH, SLOT_OFFSET(SLOT_DMACARD) + DMACARD_REGISTER_COUNTL, SLOT_OFFSET(SLOT_DMACARD)
			+ DMACARD_REGISTER_SOURCEH, SLOT_OFFSET(SLOT_DMACARD) + DMACARD_REGISTER_SOURCEL, SLOT_OFFSET(SLOT_DMACARD)
			+ DMACARD_REGISTER_DESTINATIONH, SLOT_OFFSET(SLOT_DMACARD) + DMACARD_REGISTER_DESTINATIONL };

	for (int i = 0; i < (sizeof(dmaregisternames) / sizeof(dmaregisternames[0])); i++) {
		printf("#define %s ((volatile uint16_t*) 0x%x)\n", dmaregisternames[i], dmaregisteraddresses[i]);
	}

}

static void ata() {
	printf("#define ide_register_command ((volatile uint8_t*) 0x%x)\n", SLOT_OFFSET(SLOT_CFCARD) + 14);

}

static void video() {

	uint32_t registers = VIDEO_MEMORYEND;
	registers = utils_nextpow(registers);

	printf("#define VIDEO_WIDTH 0x%x\n", VIDEO_WIDTH);
	printf("#define VIDEO_HEIGHT 0x%x\n", VIDEO_HEIGHT);
	printf("#define VIDEO_PLAYFIELDWIDTH 0x%x\n", VIDEO_PLAYFIELDWIDTH);
	printf("#define VIDEO_PLAYFIELDHEIGHT 0x%x\n", VIDEO_PLAYFIELDHEIGHT);

	char* registernames[] = { "start", "end", "register_flags", "register_config", "register_pixel", "register_line",
			"register_frame", "register_posx", "register_posy", "register_winx", "register_winy", "register_winwidth",
			"register_winheight" };

	uint32_t regoffset = SLOT_OFFSET(SLOT_VIDEOCARD) + registers;

	uint32_t registeroffsets[] = { SLOT_OFFSET(SLOT_VIDEOCARD), SLOT_OFFSET(SLOT_VIDEOCARD) + VIDEO_MEMORYEND, regoffset
			+ VIDEO_REG_FLAGS, regoffset + VIDEO_REG_CONFIG, regoffset + VIDEO_REG_PIXEL, regoffset + VIDEO_REG_LINE,
			regoffset + VIDEO_REG_FRAME, regoffset + VIDEO_REG_POSX, regoffset + VIDEO_REG_POSY,
			SLOT_OFFSET(SLOT_VIDEOCARD) + registers + VIDEO_REG_WINX, regoffset + VIDEO_REG_WINY, regoffset
					+ VIDEO_REG_WINWIDTH, regoffset + VIDEO_REG_WINHEIGHT };

	for (int reg = 0; reg < SIZEOFARRAY(registernames); reg++) {
		printf("#define video_%s ((volatile uint16_t*) 0x%x)\n", registernames[reg], registeroffsets[reg]);
	}

}

void sound() {

	uint32_t channelregisterbase;
	uint32_t channelbases[TOTALCHANNELS];

	channelregisterbase = utils_nextpow(SAMPLETOTAL);

	soundcard_channelbases(channelbases, channelregisterbase);

	for (int i = 0; i < SAMPLEPAGES; i++) {
		printf("#define sound_bank_%d ((volatile uint16_t*) 0x%x)\n", i,
				SLOT_OFFSET(SLOT_SOUNDCARD) + SAMPLEPAGESIZE * i);
	}

	for (int i = 0; i < TOTALCHANNELS; i++) {
		if (i == 0) {
			printf("#define sound_channel_master_config ((volatile uint16_t*) 0x%x)\n",
					SLOT_OFFSET(SLOT_SOUNDCARD) + channelbases[i] + SOUND_REGISTER_CONFIG);
			printf("#define sound_channel_master_volume ((volailte uint16_t*) 0x%x)\n",
					SLOT_OFFSET(SLOT_SOUNDCARD) + channelbases[i] + SOUND_REGISTER_VOLUME);
		}
		else {

			char* registers[] = { "config", "volume", "samplepointer", "samplelength", "samplepos" };
			uint32_t offsets[] = { SLOT_OFFSET(SLOT_SOUNDCARD) + channelbases[i] + SOUND_REGISTER_CONFIG,
					SLOT_OFFSET(SLOT_SOUNDCARD) + channelbases[i] + SOUND_REGISTER_VOLUME, SLOT_OFFSET(SLOT_SOUNDCARD)
							+ channelbases[i] + SOUND_REGISTER_SAMPLEPOINTER, SLOT_OFFSET(SLOT_SOUNDCARD)
							+ channelbases[i] + SOUND_REGISTER_SAMPLELENGTH, SLOT_OFFSET(SLOT_SOUNDCARD)
							+ channelbases[i] + SOUND_REGISTER_SAMPLEPOS };

			for (int reg = 0; reg < SIZEOFARRAY(registers); reg++) {
				printf("#define sound_channel_%d_%s ((volatile uint16_t*) 0x%x)\n", i - 1, registers[reg],
						offsets[reg]);
			}
		}
	}

}

void uart() {

	char* registernames[] = { "rxtx", "interruptenable", "linecontrol", "linestatus" };

	uint32_t registeroffsets[] = { SLOT_OFFSET(SLOT_UARTCARD) + UART_REGISTER_RXTXBUFFER, SLOT_OFFSET(SLOT_UARTCARD)
			+ UART_REGISTER_INTERRUPTENABLE, SLOT_OFFSET(SLOT_UARTCARD) + UART_REGISTER_LINECONTROL,
			SLOT_OFFSET(SLOT_UARTCARD) + UART_REGISTER_LINESTATUS };

	for (int reg = 0; reg < SIZEOFARRAY(registernames); reg++) {
		printf("#define uart_chan0_%s ((volatile uint8_t*) 0x%x)\n", registernames[reg], registeroffsets[reg]);
	}
}

void input() {
	printf("#define input_start ((volatile uint8_t*) 0x%x)\n", SLOT_OFFSET(SLOT_INPUTCARD));
}
