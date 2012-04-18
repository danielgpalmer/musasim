/*
 * video.c
 *
 *  Created on: Jan 19, 2012
 *      Author: daniel
 */

#include <stdint.h>
#include "include/video.h"
#include "include/video_registers.h"

void video_blitimage(int width, int height, int x, int y, void* data, dataloader loader) {

	uint16_t buff[width];
	for (int i = 0; i < height; i++) {
		loader(data, buff, sizeof(buff));
		for (int p = 0; p < width; p++) {
			*(video_start + (VIDEO_PLAYFIELDWIDTH * (i + y)) + p + x) = buff[p];
		}
	}

}
