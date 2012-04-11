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

#define PRE(tag) printf("#ifndef LIBUNAGIPAI_%s_H_\n#define LIBUNAGIPAI_%s_H_\n\n\n", tag, tag)
#define POST(tag) printf("\n#endif // %s\n", tag)

#define TAG_DMA "DMAREGISTERS"
#define TAG_ATA "ATAREGISTERS"
#define TAG_VIDEO "VIDEOREGISTERS"
#define TAG_SOUND "SOUNDREGISTERS"
#define TAG_INPUT "INPUTREGISTERS"
#define TAG_UART "UARTREGISTERS"

int main(int argc, char* argv[]) {

	if (argc != 2) {
		printf("usage; ./%s [machine|video|sound]\n", argv[0]);
	}
	else {
		if (strcmp(argv[1], "dma") == 0) {
			printf("/* dma_registers.h */\n\n");
			PRE(TAG_DMA);
			common();
			dma();
			POST(TAG_DMA);
		}
		if (strcmp(argv[1], "ata") == 0) {
			printf("/* ata_registers.h */\n\n");
			PRE(TAG_ATA);
			common();
			ata();
			POST(TAG_ATA);
		}
		else if (strcmp(argv[1], "video") == 0) {
			printf("/* video_registers.h */\n\n");
			PRE(TAG_VIDEO);
			common();
			video();
			POST(TAG_VIDEO);
		}
		else if (strcmp(argv[1], "sound") == 0) {
			printf("/* sound_registers.h */\n\n");
			PRE(TAG_SOUND);
			common();
			sound();
			POST(TAG_SOUND);
		}
		else if (strcmp(argv[1], "input") == 0) {
			printf("/* input_registers.h */\n\n");
			PRE(TAG_INPUT);
			common();
			input();
			POST(TAG_INPUT);
		}
		else if (strcmp(argv[1], "uart") == 0) {
			printf("/* uart_registers.h */\n\n");
			PRE(TAG_UART);
			common();
			uart();
			POST(TAG_UART);
		}
	}
	return 0;
}

void common() {
	printf("%s", headers);
}

static void dma() {
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
	printf("#define ata_register_command *((volatile uint8_t*) 0x%x)\n", SLOT_OFFSET(SLOT_CFCARD) + 14);

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

static void sound() {

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

static void uart() {

	char* registernames[] = { "rxtx", "interruptenable", "linecontrol", "linestatus" };

	uint8_t chanlen = 0x8;

	uint32_t registeroffsets[] = { SLOT_OFFSET(SLOT_UARTCARD) + UART_REGISTER_RXTXBUFFER, SLOT_OFFSET(SLOT_UARTCARD)
			+ UART_REGISTER_INTERRUPTENABLE, SLOT_OFFSET(SLOT_UARTCARD) + UART_REGISTER_LINECONTROL,
			SLOT_OFFSET(SLOT_UARTCARD) + UART_REGISTER_LINESTATUS };

	for (int chan = 0; chan < 2; chan++) {
		for (int reg = 0; reg < SIZEOFARRAY(registernames); reg++) {
			printf("#define uart_chan%d_%s ((volatile uint8_t*) 0x%x)\n", chan, registernames[reg],
					(chanlen * chan) + registeroffsets[reg]);
		}
	}
}

static void input() {
	printf("#define input_start ((volatile uint8_t*) 0x%x)\n", SLOT_OFFSET(SLOT_INPUTCARD));
}
