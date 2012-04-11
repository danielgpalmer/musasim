/*
 * compactflashinterfacecard.h
 *
 *  Created on: Dec 3, 2011
 *      Author: daniel
 */

#ifndef COMPACTFLASHINTERFACECARD_H_
#define COMPACTFLASHINTERFACECARD_H_

#include "card.h"
#include <stdbool.h>
bool cfintf_load(const char* filename);
const card compactflashinterfacecard;

#define BLOCKMASK 0b10000 // A4, CS1FX and CS3FX
#define ALTSTATUSOFFSET 0x06

#endif /* COMPACTFLASHINTERFACECARD_H_ */
