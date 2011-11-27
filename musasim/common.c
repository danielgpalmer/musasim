/*
 * common.c
 *
 *  Created on: Oct 3, 2011
 *      Author: daniel
 */

#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

#include "common.h"
#include "sim.h"

#include "hardware/board.h"

bool common_loadrom(char* path) {

	memset(g_rom, 0x00, SIZE_ROM);
	FILE* fhandle;
	if ((fhandle = fopen(path, "rb")) == NULL) {
		printf("Unable to open %s\n", path);
		return false;
	}

	printf("Loading %s into ROM\n", path);
	if (fread(g_rom, 1, SIZE_ROM + 1, fhandle) <= 0) {
		printf("Error reading %s\n", path);
		return false;
	}

	return true;
}
