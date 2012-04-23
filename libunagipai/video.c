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
#include "include/math.h"

static bool transactionopen = false;

void video_changewindow() {

}

void video_begin() {
	transactionopen = true;
	dma_begin();
}

void video_commit() {
	dma_commit();
	transactionopen = false;
}

static bool video_sanitycheck() {

	if (!transactionopen) {
		return false;
	}

	return true;
}

void video_plot(int x, int y, uint16_t colour) {
	if ((x < 0 || x > VIDEO_PLAYFIELDWIDTH - 1) || (y < 0 || y > VIDEO_PLAYFIELDHEIGHT - 1)) {
		return;
	}
	*(video_start + (VIDEO_PLAYFIELDWIDTH * y) + x) = colour;
}

void video_clear() {
	static uint32_t count = VIDEO_PLAYFIELDWIDTH * VIDEO_PLAYFIELDHEIGHT;
	dma_fillblock((uint32_t)video_start, 0xFFFF, count);
}

void video_fillrect(int x, int y, int width, int height) {

	//dma_fillblock_nonlinear();

}

void video_drawline(vector* v) {

	int dx, dy, err, sx, sy, e2;
	dx = abs(v->x2 - v->x1);
	dy = abs(v->y2 - v->y1);
	sx = (v->x1 < v->x2) ? 1 : -1;
	sy = (v->y1 < v->y2) ? 1 : -1;
	err = dy - dx;

	int x = v->x1;
	int y = v->y1;

	while (x != v->x2 && y != v->y2) {
		video_plot(x, y, 0xf000);

		e2 = 2 * err;
		if (e2 > -dy) {
			err = err - dy;
			x += sx;
		}
		if (e2 < dx) {
			err = err + dx;
			y += sy;

		}
	}

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
	if (usedma) {
		uint32_t count = width * height;
		dma_transfer_nonlinearblock(data, (uint32_t)(video_start + (VIDEO_PLAYFIELDWIDTH * y) + x), count, width,
				VIDEO_PLAYFIELDWIDTH - width);

	}
	else {
		for (int i = 0; i < height; i++) {
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
