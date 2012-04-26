/*
 * video.h
 *
 *  Created on: Jan 19, 2012
 *      Author: daniel
 */

#ifndef LIBUNAGIPAI_VIDEO_H_
#define LIBUNAGIPAI_VIDEO_H_

#include "math.h"

#define DATALOADERARGS (void* data, uint16_t* buff, int wanted)
typedef void (*dataloader)DATALOADERARGS;

void video_begin(void);
void video_commit(void);
void video_flip(void);
/**
 * blits an image to the frame buffer, data is supplied via your loader callback.
 */
void video_blitimage(int width, int height, int x, int y, void* data, dataloader);
/**
 * blits an image from a buffer to the framebuffer
 */
void video_blitimage_nocopy(int width, int height, int x, int y, uint16_t* data);
void video_gputs(char* string, uint8_t* font);
void video_clear();
void video_fillrect(int x, int y, int width, int height);
void video_drawline(vector* v);

#endif /* VIDEO_H_ */
