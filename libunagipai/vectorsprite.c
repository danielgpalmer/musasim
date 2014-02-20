/*
 * vectorsprite.c
 *
 *  Created on: 6 Jul 2013
 *      Author: daniel
 */

#include "vectorsprite.h"
#include "sprite.h"
#include "video.h"

void vectorsprite_draw(vectorsprite* sprite) {

	for (int i = 1; i < sprite->numpoints; i++) {

		int index = i * 2;

		int x1 = sprite->points[index - 2];
		int y1 = sprite->points[index - 1];
		int x2 = sprite->points[index];
		int y2 = sprite->points[index + 1];

		x1 = ((float) x1 * sprite->scale);
		y1 = ((float) y1 * sprite->scale);
		x2 = ((float) x2 * sprite->scale);
		y2 = ((float) y2 * sprite->scale);

		//video_drawline_withpoints(50, 50, 100, 100);

		video_drawline_withpoints(x1 + sprite->x, y1 + sprite->y, x2 + sprite->x, y2 + sprite->y);

	}

}

void vectorsprite_trace(vectorsprite* vectorsprite, sprite* tracesprite) {

}
