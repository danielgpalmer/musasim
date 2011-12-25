/*
 * dmacard.h
 *
 *  Created on: Dec 6, 2011
 *      Author: daniel
 */

#ifndef DMACARD_H_
#define DMACARD_H_

#include "card.h"

const card dmacard;

#define DMACARD_REGISTER_CONFIG 0
#define DMACARD_REGISTER_DATA 2
#define DMACARD_REGISTER_COUNTH 4
#define DMACARD_REGISTER_COUNTL 6
#define DMACARD_REGISTER_SOURCEH 8
#define DMACARD_REGISTER_SOURCEL 10
#define DMACARD_REGISTER_DESTINATIONH 12
#define DMACARD_REGISTER_DESTINATIONL 14

#endif /* DMACARD_H_ */
