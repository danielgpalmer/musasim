/*
 * rect.c
 *
 *  Created on: 16 Jun 2013
 *      Author: daniel
 */

#include "rect.h"
#include "video.h"
#include <stdint.h>

void rect_drawrect(rect* r, uint16_t colour) {
	video_fillrect(r->x, r->y, r->width, r->height, colour);
}

