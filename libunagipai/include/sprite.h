/*
 * sprite.h
 *
 *  Created on: Apr 21, 2012
 *      Author: daniel
 */

#ifndef SPRITE_H_
#define SPRITE_H_

#include <stdbool.h>

#include "image.h"

typedef struct {
	int x1;
	int y1;
	int x2;
	int y2;
} vector;

typedef struct {
	int x;
	int y;
	image* image;
} sprite;

void sprite_draw(sprite* s);
bool sprite_checkoverlap(sprite* s1, sprite* s2);

#endif /* SPRITE_H_ */
