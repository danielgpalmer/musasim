/*
 * video.h
 *
 *  Created on: Jan 19, 2012
 *      Author: daniel
 */

#ifndef LIBUNAGIPAI_VIDEO_H_
#define LIBUNAGIPAI_VIDEO_H_

#include <stdbool.h>
#include <stdint.h>
#include "math.h"

#define DATALOADERARGS (void* data, uint16_t* buff, int wanted)
typedef void (*dataloader)DATALOADERARGS;

/**
 * start a video transaction
 */

void video_begin(void);

/**
 * commit all of the pending video actions
 */

void video_commit(void);

/**
 * switches the currently displayed framebuffer, this should be done in the vblank
 */

void video_flip(void);

/**
 * blits an image to the frame buffer, data is supplied via your loader callback.
 */

void video_blitimage(int width, int height, int x, int y, void* data, dataloader);

/**
 * blits an image from a buffer to the framebuffer
 */
void video_blitimage_nocopy(int width, int height, int x, int y, uint16_t* data);
void video_gputs(char* string, uint8_t* font, int col, int row);

/**
 * fills the playfield with a single colour
 */

void video_clear(uint16_t clearcolour);

void video_fillrect(int x, int y, int width, int height, uint16_t colour);
void video_drawline(vector* v);

void video_setconfig(bool vblankint, bool hblankint);
void video_waitforvblank();

#endif /* VIDEO_H_ */
