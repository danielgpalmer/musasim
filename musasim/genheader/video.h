/* video.h */

#include <stdint.h>


volatile uint16_t* video_start = (uint16_t*) 0x200000;
volatile uint16_t* video_end = (uint16_t*) 0x23fc00;
volatile uint16_t* video_register_flags = (uint16_t*) 0x240000;
volatile uint16_t* video_register_config = (uint16_t*) 0x240002;
volatile uint16_t* video_register_frame = (uint16_t*) 0x240004;

/*
 * videoregistermasks.h
 *
 *  Created on: Dec 18, 2011
 *      Author: daniel
 */

#ifndef VIDEOREGISTERMASKS_H_
#define VIDEOREGISTERMASKS_H_

#define VIDEO_CONFIG_MODE_DISABLED 0
#define VIDEO_CONFIG_MODE_BITMAP 1
#define VIDEO_CONFIG_MODE_TILED 2
#define VIDEO_CONFIG_MODE_MASK 0b11
#define VIDEO_CONFIG_ENVBINT 1 << 2
#define VIDEO_CONFIG_ENHBINT 1 << 3

#define FLAG_VBLANK 1
#define FLAG_HBLANK 1 << 1

#endif /* VIDEOREGISTERMASKS_H_ */
