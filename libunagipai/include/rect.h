/*
 * rect.h
 *
 *  Created on: 16 Jun 2013
 *      Author: daniel
 */

#ifndef RECT_H_
#define RECT_H_

#include <stdint.h>

typedef struct {
	int x;
	int y;
	int width;
	int height;
} rect;

void rect_drawrect(rect* r, uint16_t colour);

#endif /* RECT_H_ */
