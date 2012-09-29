/*
 * inputcard.h
 *
 *  Created on: Dec 3, 2011
 *      Author: daniel
 */

#ifndef INPUTCARD_H_
#define INPUTCARD_H_

#include "card.h"

#define INPUT_PORT0		0
#define INPUT_PORT1		(1 << 1)
#define INPUT_RNG		(2 << 1)
#define INPUT_DIPS		(3 << 1)
#define INPUT_DEBUGLEDS	(4 << 1)

const card inputcard;

SDL_Surface* inputcard_getledssurface(void);

#endif /* INPUTCARD_H_ */
