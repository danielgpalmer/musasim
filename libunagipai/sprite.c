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

static bool checksides(sprite* s1, sprite* s2) {
	if (s2->y >= s1->y && s2->y <= (s1->y + s1->image->height)) { // top edge inside
		return true;
	}

	else if ((s2->y + s2->image->height) >= s1->y && (s2->y + s2->image->height) <= (s1->y + s1->image->height)) { // bottom edge
		return true;
	}

	return false;
}

bool sprite_checkoverlap(sprite* s1, sprite* s2) {

	if (s2->x >= s1->x && s2->x <= (s1->x + s1->image->width)) { // left edge is inside
		return checksides(s1, s2);

	}

	else if ((s2->x + s2->image->width) >= s1->x && (s2->x + s2->image->width) <= (s1->x + s1->image->width)) { // right edge is inside
		return checksides(s1, s2);
	}

	return false;
}

