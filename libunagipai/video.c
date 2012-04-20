/*
 * video.c
 *
 *  Created on: Jan 19, 2012
 *      Author: daniel
 */

#include <stdint.h>
#include <stdbool.h>
#include "include/dma.h"
#include "include/video.h"
#include "include/video_registers.h"

void video_begin() {

}

void video_commit() {

}

void video_clear() {
	static uint32_t count = VIDEO_PLAYFIELDWIDTH * VIDEO_PLAYFIELDHEIGHT;

	dma_begin();
	dma_fillblock(0, 0xFFFF, count);
	dma_commit();

}

void video_blitimage(int width, int height, int x, int y, void* data, dataloader loader) {

	uint16_t buff[width];
	for (int i = 0; i < height; i++) {
		loader(data, buff, sizeof(buff));
		for (int p = 0; p < width; p++) {
			*(video_start + (VIDEO_PLAYFIELDWIDTH * (i + y)) + p + x) = buff[p];
		}
	}

}

void video_blitimage_nocopy(int width, int height, int x, int y, uint16_t* data) {

	bool usedma = true;

	for (int i = 0; i < height; i++) {
		if (usedma) {
			dma_begin();
			dma_transferblock(data + (width * i), video_start + (VIDEO_PLAYFIELDWIDTH * (i + y)) + x, width);
			dma_commit();
		}
		else {
			for (int p = 0; p < width; p++) {
				*(video_start + (VIDEO_PLAYFIELDWIDTH * (i + y)) + p + x) = data[(i * width) + p];
			}
		}
	}

}

#define CHARWIDTH 8
#define CHARHEIGHT 16
#define COLS (VIDEO_PLAYFIELDWIDTH/CHARWIDTH)
#define ROWSIZE (COLS * CHARWIDTH * CHARHEIGHT)

void video_gputs(char* string, uint8_t* font) {

	static int col = 0;
	static int row = 0;
	int charoffset = 32 * CHARHEIGHT;
	char c = 0;

	while ((c = *string++) != 0) {
		for (int i = 0; i < CHARHEIGHT; i++) {

			uint8_t character = font[i + ((c * CHARHEIGHT) - charoffset)];

			for (int j = 0; j < CHARWIDTH; j++) {

				int offset = (ROWSIZE * row) + ((VIDEO_PLAYFIELDWIDTH * i) + j + (col * CHARWIDTH));

				int pixel = character & 0x01;
				if (pixel) {
					*(video_start + offset) = 0x0000;
				}
				else {
					*(video_start + offset) = 0xFFFF;
				}
				character = (character >> 1);
			}
		}

		col++;
	}
}
