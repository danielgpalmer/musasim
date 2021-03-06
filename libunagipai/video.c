/*
 * video.c
 *
 *  Created on: Jan 19, 2012
 *      Author: daniel
 */

#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>
#include "include/dma.h"
#include "include/video.h"
#include "include/video_registers.h"
#include "include/video_registermasks.h"
#include "include/math.h"
#include "include/utils.h"

static bool transactionopen = false;

void video_flip() {
	uint16_t config = video_register_config;
	if (config & VIDEO_CONFIG_FLIP) {
		video_register_config = config & ~VIDEO_CONFIG_FLIP;
	}
	else {
		video_register_config = config | VIDEO_CONFIG_FLIP;
	}
}

void video_waitforvblank() {
	while (!(video_register_flags & FLAG_VBLANK)) {
	}
}

void video_setconfig(bool vblankint, bool hblankint) {

	uint16_t flags = VIDEO_CONFIG_MODE_BITMAP;
	if (vblankint)
		flags |= VIDEO_CONFIG_ENVBINT;
	if (hblankint)
		flags |= VIDEO_CONFIG_ENHBINT;

	video_register_config = flags;
}

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
	*(video_framebuffer + (VIDEO_PLAYFIELDWIDTH * y) + x) = colour;
}

void video_clear(uint16_t clearcolour) {
	static uint32_t count = VIDEO_PLAYFIELDWIDTH * VIDEO_PLAYFIELDHEIGHT;
	dma_fillblock((uint32_t) video_framebuffer, clearcolour, count);
}

void video_fillrect(int x, int y, int width, int height, uint16_t colour) {

	uint32_t start = ((uint32_t) video_framebuffer) + (y * VIDEO_LINESIZE) + (x * VIDEO_PIXELSIZE);
	uint32_t count = width * height;
	uint16_t jumpafter = width;
	uint16_t jumplength = VIDEO_PLAYFIELDWIDTH - width;
	dma_fillblock_nonlinear(start, colour, count, jumpafter, jumplength);
//dma_fillblock_nonlinear();

}

void video_drawline(vector* v) {
	video_drawline_withpoints(v->x1, v->y1, v->x2, v->y2);
}

void video_drawline_withpoints(int x1, int y1, int x2, int y2) {

	x1 = MIN(x1, VIDEO_PLAYFIELDWIDTH);
	x2 = MIN(x2, VIDEO_PLAYFIELDWIDTH);
	y1 = MIN(y1, VIDEO_PLAYFIELDWIDTH);
	y2 = MIN(y2, VIDEO_PLAYFIELDWIDTH);

	dma_commit();

	int dx, dy, err, sx, sy, e2;
	dx = abs(x2 - x1);
	dy = abs(y2 - y1);
	sx = (x1 < x2) ? 1 : -1;
	sy = (y1 < y2) ? 1 : -1;
	err = dy - dx;

	int x = x1;
	int y = y1;

	while (x != x2 && y != y2) {
		video_plot(x, y, 0xf000);

		e2 = 2 * err;
		if (e2 > -dy) {
			err = err - dy;
			x += sx;
		}
		if (x == x2 && y == y2) {
			video_plot(x, y, 0xf000);
			break;
		}
		if (e2 < dx) {
			err = err + dx;
			y += sy;

		}
	}

	dma_begin();
}

void video_blitimage(int width, int height, int x, int y, void* data, dataloader loader) {

	uint16_t buff[width];
	for (int i = 0; i < height; i++) {
		loader(data, buff, sizeof(buff));
		for (int p = 0; p < width; p++) {
			*(video_framebuffer + (VIDEO_PLAYFIELDWIDTH * (i + y)) + p + x) = buff[p];
		}
	}

}

void video_blitimage_nocopy(int width, int height, int x, int y, uint16_t* data) {

	bool usedma = true;
	if (usedma) {
		uint32_t count = width * height;
		dma_transfer_nonlinearblock((uint32_t) data, (uint32_t)(video_framebuffer + (VIDEO_PLAYFIELDWIDTH * y) + x),
				count, width, VIDEO_PLAYFIELDWIDTH - width);

	}
	else {
		for (int i = 0; i < height; i++) {
			for (int p = 0; p < width; p++) {
				*(video_framebuffer + (VIDEO_PLAYFIELDWIDTH * (i + y)) + p + x) = data[(i * width) + p];
			}
		}
	}

}

#define CHARWIDTH 8
#define CHARHEIGHT 16
#define COLS (VIDEO_PLAYFIELDWIDTH/CHARWIDTH)
#define ROWSIZE (COLS * CHARWIDTH * CHARHEIGHT)

void video_gputs(char* string, uint8_t* font, int col, int row) {

	dma_commit();

	int charoffset = 32 * CHARHEIGHT;
	char c = 0;

	while ((c = *string++) != 0) {
		for (int i = 0; i < CHARHEIGHT; i++) {

			uint8_t character = font[i + ((c * CHARHEIGHT) - charoffset)];

			for (int j = 0; j < CHARWIDTH; j++) {

				int offset = (ROWSIZE * row) + ((VIDEO_PLAYFIELDWIDTH * i) + j + (col * CHARWIDTH));

				int pixel = character & 0x01;
				if (pixel) {
					*(video_framebuffer + offset) = 0x0000;
				}
				else {
					*(video_framebuffer + offset) = 0xFFFF;
				}
				character = (character >> 1);
			}
		}

		col++;
	}

	dma_begin();
}
