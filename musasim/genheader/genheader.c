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
#include "../hardware/video.h"

char headers[] = "#include <stdint.h>\n"
		"\n\n";

void common();
void machine();
void video();
void sound();

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
	}
	return 0;
}

void common() {
	printf("%s", headers);
}

void machine() {
	printf("volatile uint16_t* machine_ram_start = (uint16_t*) 0x%x;\n", OFFSET_RAM);
	printf("volatile uint16_t* machine_ram_end = (uint16_t*) 0x%x;\n", MAX_RAM);
	printf("uint16_t machine_ram_size = (uint16_t) 0x%x;\n", SIZE_RAM);
}

void video() {
	printf("volatile uint16_t* video_start = (uint16_t*) 0x%x;\n", OFFSET_VIDEO);
	printf("volatile uint16_t* video_end = (uint16_t*) 0x%x;\n", MAX_VIDEO);
	printf("volatile uint16_t* video_registers = (uint16_t*) 0x%x;\n", OFFSET_VIDEOREGISTERS);
}

void sound() {

}
