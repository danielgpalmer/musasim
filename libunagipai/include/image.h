/*
 * image.h
 *
 *  Created on: Apr 21, 2012
 *      Author: daniel
 */

#ifndef IMAGE_H_
#define IMAGE_H_

#include <stdbool.h>
#include <stdint.h>
#include "pff.h"

typedef struct {
	uint16_t width;
	uint16_t height;
	uint16_t* data;
} image;

typedef struct {
	uint16_t width;
	uint16_t height;
	uint16_t* imagedata;
	uint16_t* maskdata;
} maskedimage;

void image_loadimagefromfile(FATFS* fs, image* image, const char* path, bool compressed);

void image_loadmaskedimagefromfile(FATFS* fs, image* image, const char* imagepath, const char* maskpath,
		bool compressed);

#endif /* IMAGE_H_ */
