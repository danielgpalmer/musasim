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
#include <glib.h>

#include "../sim.h"
#include "../hardware/board.h"
#include "../hardware/cards/romcard.h"
#include "../hardware/cards/videocard.h"
#include "../hardware/cards/uartcard.h"
#include "../hardware/cards/dmacard.h"
#include "../hardware/cards/inputcard.h"
#include "../hardware/cards/compactflashinterfacecard.h"
#define WANTSOUNDFUNC
#include "../hardware/cards/soundcard.h"
#include "../hardware/cards/timercard.h"
#include "../hardware/registerplanner.h"
#include "../utils.h"

char headers[] = "#include <stdint.h>\n"
		"\n\n";

static void common(void);
static void machine(void);
static void dma(void);
static void ata(void);
static void video(void);
static void sound(void);
static void input(void);
static void uart(void);
static void timers(void);

#define PRE(tag) printf("#ifndef LIBUNAGIPAI_%s_H_\n#define LIBUNAGIPAI_%s_H_\n\n\n", tag, tag)
#define POST(tag) printf("\n#endif // %s\n", tag)

#define TAG_MACHINE "MACHINESTUFF"
#define TAG_DMA "DMAREGISTERS"
#define TAG_ATA "ATAREGISTERS"
#define TAG_VIDEO "VIDEOREGISTERS"
#define TAG_SOUND "SOUNDREGISTERS"
#define TAG_INPUT "INPUTREGISTERS"
#define TAG_UART "UARTREGISTERS"
#define TAG_TIMERS "TIMERSREGISTERS"

char* prefix = NULL;
uint32_t offset = 0;

static void fileheader(char* filename, char* brief) {
	printf("/**\n");
	printf(" *   \\file %s\n", filename);
	printf(" *   \\author genheader\n");
	printf(" *   \\brief %s\n", brief);
	printf(" */\n");
}

#ifdef main
# undef main
#endif

int main(int argc, char* argv[]) {

	if (argc != 2) {
		printf("usage; ./%s [machine|dma|ata|video|sound|input|uart|timers]\n",
				argv[0]);
	} else {
		if (strcmp(argv[1], "machine") == 0) {
			fileheader("machine.h", "Machine helper defines");
			PRE(TAG_MACHINE);
			common();
			machine();
			POST(TAG_MACHINE);
		} else if (strcmp(argv[1], "dma") == 0) {
			fileheader("dma_registers.h", "DMA register defines");
			PRE(TAG_DMA);
			common();
			dma();
			POST(TAG_DMA);
		} else if (strcmp(argv[1], "ata") == 0) {
			fileheader("ata_registers.h", "ATA register defines");
			PRE(TAG_ATA);
			common();
			ata();
			POST(TAG_ATA);
		} else if (strcmp(argv[1], "video") == 0) {
			fileheader("video_registers.h", "Video register defines");
			PRE(TAG_VIDEO);
			common();
			video();
			POST(TAG_VIDEO);
		} else if (strcmp(argv[1], "sound") == 0) {
			fileheader("sound_registers.h", "Sound register defines");
			PRE(TAG_SOUND);
			common();
			sound();
			POST(TAG_SOUND);
		} else if (strcmp(argv[1], "input") == 0) {
			fileheader("input_registers.h", "Input register defines");
			PRE(TAG_INPUT);
			common();
			input();
			POST(TAG_INPUT);
		} else if (strcmp(argv[1], "uart") == 0) {
			fileheader("uart_registers.h", "UART register defines");
			PRE(TAG_UART);
			common();
			uart();
			POST(TAG_UART);
		} else if (strcmp(argv[1], "timers") == 0) {
			fileheader("timers_registers.h", "Timers register defines");
			PRE(TAG_TIMERS);
			common();
			timers();
			POST(TAG_TIMERS);
		}
	}
	return 0;
}

void common() {
	printf("%s", headers);
}

static void machine() {
	uint32_t offset = SLOT_OFFSET(SLOT_ROMCARD);
	printf("#define ROMDISABLE0 0x%x\n", ROMDISABLE_DATA_0);
	printf("#define ROMDISABLE1 0x%x\n", ROMDISABLE_DATA_1);
	printf("#define romdisable0 (*(volatile uint8_t*) 0x%x)\n",
			ROMDISABLE_ADDRESS_0);
	printf("#define romdisable1 (*(volatile uint8_t*) 0x%x)\n",
			ROMDISABLE_ADDRESS_1);

}

static void dma() {
	//printf("volatile uint16_t* machine_ram_start = (uint16_t*) 0x%x;\n", OFFSET_RAM);
	//printf("volatile uint16_t* machine_ram_end = (uint16_t*) 0x%x;\n", MAX_RAM);
	//printf("uint16_t machine_ram_size = (uint16_t) 0x%x;\n", SIZE_RAM);

	char* dmaregisternames[] = { "dma_register_config", "dma_register_datah",
			"dma_register_datal", "dma_register_counth", "dma_register_countl",
			"dma_register_srch", "dma_register_srcl", "dma_register_desth",
			"dma_register_destl", "dma_register_jumpafter",
			"dma_register_jumplength", "dma_register_window" };

	uint32_t offset = SLOT_OFFSET(SLOT_DMACARD);

	uint32_t dmaregisteraddresses[] = { offset + DMACARD_REGISTER_CONFIG, offset
			+ DMACARD_REGISTER_DATAH, offset + DMACARD_REGISTER_DATAL, offset
			+ DMACARD_REGISTER_COUNTH, offset + DMACARD_REGISTER_COUNTL, offset
			+ DMACARD_REGISTER_SOURCEH, offset + DMACARD_REGISTER_SOURCEL,
			offset + DMACARD_REGISTER_DESTINATIONH, offset
					+ DMACARD_REGISTER_DESTINATIONL, offset
					+ DMACARD_REGISTER_JUMPAFTER, offset
					+ DMACARD_REGISTER_JUMPLENGTH, offset
					+ DMACARD_REGISTER_WINDOW };

	for (int i = 0;
			i < (sizeof(dmaregisternames) / sizeof(dmaregisternames[0])); i++) {
		printf("#define %s (*(volatile uint16_t*) 0x%x)\n", dmaregisternames[i],
				dmaregisteraddresses[i]);
	}

}

static void ata() {
	uint32_t slotoffset = SLOT_OFFSET(SLOT_CFCARD);
	printf("#define ata_register_data *((volatile uint16_t*) 0x%x)\n",
			slotoffset);

	char* regnames[] = { "errorfeature", "sectorcount", "sectornumber",
			"cylinderlow", "cylinderhigh", "drivehead", "command", "altstatus" };
	uint32_t regoffsets[] = { slotoffset + (ERRORFEATURE << 1), slotoffset
			+ (SECTORCOUNT << 1), slotoffset + (SECTORNUMBER << 1), slotoffset
			+ (CYLINDERLOW << 1), slotoffset + (CYLINDERHIGH << 1), slotoffset
			+ (DRIVEHEAD << 1), slotoffset + (COMMANDSTATUS << 1), slotoffset
			+ BLOCKMASK + (ALTSTATUSOFFSET << 1) };

	for (int reg = 0; reg < G_N_ELEMENTS(regnames); reg++) {
		printf("#define ata_register_%s *((volatile uint8_t*) 0x%x)\n",
				regnames[reg], regoffsets[reg]);
	}

}

static void video() {

	printf("#define VIDEO_WIDTH 0x%x\n", VIDEO_WIDTH);
	printf("#define VIDEO_HEIGHT 0x%x\n", VIDEO_HEIGHT);
	printf("#define VIDEO_PLAYFIELDWIDTH 0x%x\n", VIDEO_PLAYFIELDWIDTH);
	printf("#define VIDEO_PLAYFIELDHEIGHT 0x%x\n", VIDEO_PLAYFIELDHEIGHT);
	printf("#define VIDEO_PIXELSIZE 0x%x\n", 2);
	printf("#define VIDEO_LINESIZE 0x%x\n", VIDEO_PLAYFIELDWIDTH * 2);

	char* registernames[] = { "register_flags", "register_config",
			"register_pixel", "register_line", "register_frame",
			"register_posx", "register_posy", "register_winx", "register_winy",
			"register_winwidth", "register_winheight" };

	uint32_t registeroffsets[] = { VIDEO_REG_FLAGS, VIDEO_REG_CONFIG,
			VIDEO_REG_PIXEL, VIDEO_REG_LINE, VIDEO_REG_FRAME,
			VIDEO_REG_POSX, VIDEO_REG_POSY, VIDEO_REG_WINX, VIDEO_REG_WINY,
			VIDEO_REG_WINWIDTH, VIDEO_REG_WINHEIGHT };

	for (int reg = 0; reg < G_N_ELEMENTS(registernames); reg++) {
		printf("#define video_%s (*(volatile uint16_t*) 0x%x)\n",
				registernames[reg],
				SLOT_OFFSET(SLOT_VIDEOCARD) + registeroffsets[reg]);
	}

	// buffers
	printf("#define video_framebuffer ((volatile uint16_t*) 0x%x)\n",
	SLOT_OFFSET(SLOT_VIDEOCARD) + VIDEO_FRAMEBUFFER_START);
	printf("#define video_compositingbuffer ((volatile uint16_t*) 0x%x)\n",
	SLOT_OFFSET(SLOT_VIDEOCARD) + VIDEO_COMPOSITINGBUFFER_START);

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
			printf(
					"#define sound_channel_master_config (*(volatile uint16_t*) 0x%x)\n",
					SLOT_OFFSET(
							SLOT_SOUNDCARD) + channelbases[i] + SOUND_REGISTER_CONFIG);
			printf(
					"#define sound_channel_master_volume (*(volatile uint16_t*) 0x%x)\n",
					SLOT_OFFSET(
							SLOT_SOUNDCARD) + channelbases[i] + SOUND_REGISTER_VOLUME);
		} else {

			char* registers[] = { "config", "volume", "samplepointer",
					"samplelength", "samplepos" };
			uint32_t offsets[] =
					{
							SLOT_OFFSET(
									SLOT_SOUNDCARD) + channelbases[i] + SOUND_REGISTER_CONFIG,
							SLOT_OFFSET(
									SLOT_SOUNDCARD) + channelbases[i] + SOUND_REGISTER_VOLUME,
							SLOT_OFFSET(
									SLOT_SOUNDCARD) + channelbases[i] + SOUND_REGISTER_SAMPLEPOINTER,
							SLOT_OFFSET(
									SLOT_SOUNDCARD) + channelbases[i] + SOUND_REGISTER_SAMPLELENGTH,
							SLOT_OFFSET(SLOT_SOUNDCARD) + channelbases[i]
									+ SOUND_REGISTER_SAMPLEPOS };

			for (int reg = 0; reg < G_N_ELEMENTS(registers); reg++) {
				printf(
						"#define sound_channel_%d_%s (*(volatile uint16_t*) 0x%x)\n",
						i - 1, registers[reg], offsets[reg]);
			}
		}
	}

}

static void uart() {

	char* registernames[] = { "rxtx", "interruptenable", "fifocontrol",
			"linecontrol", "modemcontrol", "linestatus", "modemstatus",
			"stratch" };

	uint8_t chanlen = 0x10;

	uint32_t offset = SLOT_OFFSET(SLOT_UARTCARD);

	uint32_t registeroffsets[] =
			{ offset + UART_REGISTER_RXTXBUFFER, offset
					+ UART_REGISTER_INTERRUPTENABLE, offset
					+ UART_REGISTER_FIFOCONTROL, offset
					+ UART_REGISTER_LINECONTROL, offset
					+ UART_REGISTER_MODEMCONTROL, offset
					+ UART_REGISTER_LINESTATUS, offset
					+ UART_REGISTER_MODEMSTATUS, offset + UART_REGISTER_SCRATCH };

	for (int chan = 0; chan < 2; chan++) {
		for (int reg = 0; reg < G_N_ELEMENTS(registernames); reg++) {
			printf("#define uart_chan%d_%s (*(volatile uint8_t*) 0x%x)\n", chan,
					registernames[reg],
					(chanlen * chan) + registeroffsets[reg]);
		}
	}
}

static void input() {
	uint32_t offset = SLOT_OFFSET(SLOT_INPUTCARD);
	printf("#define input_port0 (*(volatile uint8_t*) 0x%x)\n",
			offset + INPUT_PORT0);
	printf("#define input_port1 (*(volatile uint8_t*) 0x%x)\n",
			offset + INPUT_PORT1);
	printf("#define input_rng (*(volatile uint8_t*) 0x%x)\n",
			offset + INPUT_RNG);
	printf("#define input_dips (*(volatile uint8_t*) 0x%x)\n",
			offset + INPUT_DIPS);
	printf("#define input_debugleds (*(volatile uint8_t*) 0x%x)\n",
			offset + INPUT_DEBUGLEDS);

}

static void timers_registerfunc(uint32_t address, int size, const char* name,
		void* data) {
	char* unitname = (char*) data;
	char* type;

	switch (size) {
	case 4:
		type = "uint32_t";
		break;
	case 2:
		type = "uint16_t";
		break;
	case 1:
		type = "uint8_t";
		break;
	}

	printf("#define %s_%s_%s (*(volatile %s*) 0x%x)\n", prefix, unitname, name,
			type, offset + address);
}

static void timers_func(unit* unit) {
	printf("/* %s */\n", unit->either.name);
	registerplanner_iterate_registers(unit, timers_registerfunc,
			(void*) unit->either.name);
	printf("\n");
}

static void timers() {
	offset = SLOT_OFFSET(SLOT_TIMERCARD);
	cardaddressspace* as = timercard.setupaddressspace();

	printf("/**\n");
	registerplanner_print(as);
	printf("**/\n");

	printf("#define timers_timerinterrupts (*(volatile uint16_t*) 0x%x)\n",
			offset);
	prefix = "timers";
	registerplanner_iterate(as, timers_func);
	printf("\n");
}
