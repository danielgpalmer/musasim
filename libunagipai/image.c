/*
 * image.c
 *
 *  Created on: Apr 21, 2012
 *      Author: daniel
 */

#include "include/image.h"

#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>

#include "include/lzfx.h"
#include "include/pff.h"

uint8_t* image_loadimagefromfile(FATFS* fs, const char* path, uint16_t* width, uint16_t* height, bool compressed) {
	FRESULT result;
	int len;

	// open the file
	result = pf_open(path);

	// get the width and height
	pf_read(width, 2, &len);
	pf_read(height, 2, &len);

	// get the size of the file if this is a compressed image
	unsigned cdatalen = fs->fsize - 4;

	// calculate the size of the raw data
	unsigned int rlen = 2 * (*(width) * *(height));

	printf("compressed image is %u * %u, compressed data is %u bytes\n", (unsigned) *width, (unsigned) *height,
			cdatalen);

	uint8_t* rawdata = malloc(rlen);

	if (compressed) {
		uint8_t* compresseddata = malloc(cdatalen);
		pf_read(compresseddata, cdatalen, &len);
		if (lzfx_decompress(compresseddata, cdatalen, rawdata, &rlen) == 0) {
			printf("decompressed!\n");
			free(compresseddata);
		}
	}
	else {
		pf_read(rawdata, rlen, &len);
	}

	return rawdata;

}
