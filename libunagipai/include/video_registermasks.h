/*
 * videoregistermasks.h
 *
 *  Created on: Dec 18, 2011
 *      Author: daniel
 */

#ifndef VIDEOREGISTERMASKS_H_
#define VIDEOREGISTERMASKS_H_

/*
 * H V MM
 *
 * MM - mode
 * V - Vertical blank interrupt enable
 * H - Horizontal blank interrupt enable
 */

#define VIDEO_CONFIG_MODE_DISABLED 0
#define VIDEO_CONFIG_MODE_BITMAP 1
#define VIDEO_CONFIG_MODE_TILED 2
#define VIDEO_CONFIG_MODE_MASK 0b11
#define VIDEO_CONFIG_ENVBINT (1 << 2)
#define VIDEO_CONFIG_ENHBINT (1 << 3)
#define VIDEO_CONFIG_FLIP (1 << 15)

/*
 * H V
 *
 * V - in vblank
 * h - in hblank
 */

#define FLAG_VBLANK 1
#define FLAG_HBLANK (1 << 1)

#endif /* VIDEOREGISTERMASKS_H_ */
