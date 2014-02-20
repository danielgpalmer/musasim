/*
 * vectorsprite.h
 *
 *  Created on: 6 Jul 2013
 *      Author: daniel
 */

#ifndef VECTORSPRITE_H_
#define VECTORSPRITE_H_

typedef struct {
	int x;
	int y;
	int numpoints;
	int* points;
	float scale;
} vectorsprite;

void vectorsprite_draw(vectorsprite* sprite);

#endif /* VECTORSPRITE_H_ */
