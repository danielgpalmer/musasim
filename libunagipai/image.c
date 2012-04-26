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

void image_loadimagefromfile(FATFS* fs, image* image, const char* path, bool compressed) {
	FRESULT result;
	uint32_t len;

	// open the file
	result = pf_open(path);

	// get the width and height
	pf_read(&(image->width), 2, &len);
	pf_read(&(image->height), 2, &len);

	// get the size of the file if this is a compressed image
	unsigned cdatalen = fs->fsize - 4;

	// calculate the size of the raw data
	unsigned int rlen = 2 * (image->width * image->height);

	printf("compressed image is %u * %u, compressed data is %u bytes\n", (unsigned) image->width,
			(unsigned) image->height, cdatalen);

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

	image->data = rawdata;

}
