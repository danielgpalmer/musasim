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

void machine() {
	//printf("volatile uint16_t* machine_ram_start = (uint16_t*) 0x%x;\n", OFFSET_RAM);
	//printf("volatile uint16_t* machine_ram_end = (uint16_t*) 0x%x;\n", MAX_RAM);
	//printf("uint16_t machine_ram_size = (uint16_t) 0x%x;\n", SIZE_RAM);

	printf("#define ide_register_command ((volatile uint8_t*) 0x%x)\n", SLOT_OFFSET(SLOT_CFCARD) + 14);

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

void video() {

	uint32_t registers = VIDEO_MEMORYEND;
	registers = utils_nextpow(registers);

	printf("#define VIDEO_WIDTH 0x%x\n", VIDEO_WIDTH);
	printf("#define VIDEO_HEIGHT 0x%x\n", VIDEO_HEIGHT);
	printf("#define VIDEO_PLAYFIELDWIDTH 0x%x\n", VIDEO_PLAYFIELDWIDTH);
	printf("#define VIDEO_PLAYFIELDHEIGHT 0x%x\n", VIDEO_PLAYFIELDHEIGHT);

	printf("#define video_start ((volatile uint16_t*) 0x%x)\n", SLOT_OFFSET(SLOT_VIDEOCARD));
	printf("#define video_end ((volatile uint16_t*) 0x%x)\n", SLOT_OFFSET(SLOT_VIDEOCARD) + VIDEO_MEMORYEND);
	printf("#define video_register_flags ((volatile uint16_t*) 0x%x)\n",
			SLOT_OFFSET(SLOT_VIDEOCARD) + registers + VIDEO_REG_FLAGS);
	printf("#define video_register_config ((volatile uint16_t*) 0x%x)\n",
			SLOT_OFFSET(SLOT_VIDEOCARD) + registers + VIDEO_REG_CONFIG);
	printf("#define video_register_pixel ((volatile uint16_t*) 0x%x)\n",
			SLOT_OFFSET(SLOT_VIDEOCARD) + registers + VIDEO_REG_PIXEL);
	printf("#define video_register_line ((volatile uint16_t*) 0x%x)\n",
			SLOT_OFFSET(SLOT_VIDEOCARD) + registers + VIDEO_REG_LINE);
	printf("#define video_register_frame ((volatile uint16_t*) 0x%x)\n",
			SLOT_OFFSET(SLOT_VIDEOCARD) + registers + VIDEO_REG_FRAME);
	printf("#define video_register_posx ((volatile uint16_t*) 0x%x)\n",
			SLOT_OFFSET(SLOT_VIDEOCARD) + registers + VIDEO_REG_POSX);
	printf("#define video_register_posy ((volatile uint16_t*) 0x%x)\n",
			SLOT_OFFSET(SLOT_VIDEOCARD) + registers + VIDEO_REG_POSY);
	printf("#define video_register_winx ((volatile uint16_t*) 0x%x)\n",
			SLOT_OFFSET(SLOT_VIDEOCARD) + registers + VIDEO_REG_WINX);
	printf("#define video_register_winy ((volatile uint16_t*) 0x%x)\n",
			SLOT_OFFSET(SLOT_VIDEOCARD) + registers + VIDEO_REG_WINY);
	printf("#define video_register_winwidth ((volatile uint16_t*) 0x%x)\n",
			SLOT_OFFSET(SLOT_VIDEOCARD) + registers + VIDEO_REG_WINWIDTH);
	printf("#define video_register_winheight ((volatile uint16_t*) 0x%x)\n",
			SLOT_OFFSET(SLOT_VIDEOCARD) + registers + VIDEO_REG_WINHEIGHT);
}

void sound() {

	uint32_t channelregisterbase;
	uint32_t channelbases[TOTALCHANNELS];

	channelregisterbase = utils_nextpow(SAMPLETOTAL);

	soundcard_channelbases(channelbases, channelregisterbase);

	for (int i = 0; i < SAMPLEPAGES; i++) {
		printf("volatile uint16_t* const sound_bank_%d = (uint16_t*) 0x%x;\n", i,
				SLOT_OFFSET(SLOT_SOUNDCARD) + SAMPLEPAGESIZE * i);
	}

	for (int i = 0; i < TOTALCHANNELS; i++) {
		if (i == 0) {
			printf("volatile uint16_t* const sound_channel_master_config = (uint16_t*) 0x%x;\n",
					SLOT_OFFSET(SLOT_SOUNDCARD) + channelbases[i] + SOUND_REGISTER_CONFIG);
			printf("volatile uint16_t* const sound_channel_master_volume = (uint16_t*) 0x%x;\n",
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
	printf("#define uart_chan0_rxtx ((volatile uint8_t*) 0x%x)\n",
			SLOT_OFFSET(SLOT_UARTCARD) + UART_REGISTER_RXTXBUFFER);
	printf("#define uart_chan0_interruptenable (( volatile uint8_t*) 0x%x)\n",
			SLOT_OFFSET(SLOT_UARTCARD) + UART_REGISTER_INTERRUPTENABLE);
	printf("#define uart_chan0_linecontrol ((volatile uint8_t*) 0x%x)\n",
			SLOT_OFFSET(SLOT_UARTCARD) + UART_REGISTER_LINECONTROL);
	printf("#define uart_chan0_linestatus ((volatile uint8_t*) 0x%x)\n",
			SLOT_OFFSET(SLOT_UARTCARD) + UART_REGISTER_LINESTATUS);

}

void input() {
	printf("volatile uint8_t* const input_start = (uint8_t*) 0x%x;\n", SLOT_OFFSET(SLOT_INPUTCARD));
}
