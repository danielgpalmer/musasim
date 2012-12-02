/*
 * utils.c
 *
 *  Created on: Apr 13, 2012
 *      Author: daniel
 */

#include <stdint.h>
#include <stdio.h>

#include "include/pff.h"

void util_hexblockprint(void* buffer, unsigned len) {

	uint8_t* bytes = (uint8_t*) buffer;

	printf("len is %d\n", len);

	for (unsigned i = 0; i < len; i += 0x10) {
		printf("%x \t- ", (unsigned) i);
		for (unsigned j = 0; j < 0x10; j++) {
			printf("0x%02x ", (unsigned) bytes[i + j]);
		}
		printf("\n");
	}

}

void util_printfat(FATFS* fs) {
	printf("fat base : 0x%08x\n", (unsigned) fs->fatbase);
	printf("data base : 0x%08x\n", (unsigned) fs->database);
}

void util_printffresult(FRESULT result) {
	switch (result) {
		case FR_OK:
			printf("OK!\n");
			break;
		case FR_DISK_ERR:
			printf("DISK ERROR!\n");
			break;
		case FR_NOT_READY:
			printf("NOT READY!\n");
			break;
		case FR_NO_FILE:
			printf("NO FILE!\n");
			break;
		case FR_NO_PATH:
			printf("NO PATH\n"); // wtf does no path mean heh
			break;
		case FR_NOT_OPENED:
			printf("FILE NOT OPENED\n");
			break;
		case FR_NOT_ENABLED:
			printf("NOT ENABLED\n");
			break;
		case FR_NO_FILESYSTEM:
			printf("NO FILESYSTEM\n");
			break;
	}
}
