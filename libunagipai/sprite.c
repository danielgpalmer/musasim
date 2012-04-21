/*
 * sprite.c
 *
 *  Created on: Apr 21, 2012
 *      Author: daniel
 */

#include "include/sprite.h"
#include "include/image.h"
#include <stdio.h>

#include <stdbool.h>

void sprite_draw(sprite* s) {
	video_blitimage_nocopy(s->image->width, s->image->height, s->x, s->y, s->image->data);
}

bool sprite_checkoverlap(sprite* s1, sprite* s2) {

	if ((s2->x >= s1->x && s2->x <= (s1->x + s1->image->width))
			|| ((s2->x + s2->image->width) >= s1->x && (s2->x + s2->image->width) <= (s1->x + s1->image->width))) {
		if ((s2->y >= s1->y && s2->y <= (s1->y + s1->image->height))
				|| ((s2->y + s2->image->height) >= s1->y && (s2->y + s2->image->height) <= (s1->y + s1->image->height))) {
			return true;
		}
	}
	return false;
}

