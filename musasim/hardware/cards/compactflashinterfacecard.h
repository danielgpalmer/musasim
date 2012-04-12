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
#define ERRORFEATURE 0x1
#define SECTORCOUNT 0x02
#define SECTORNUMBER 0x03
#define CYLINDERLOW 0x04
#define CYLINDERHIGH 0x05
#define DRIVEHEAD 0x06
#define ALTSTATUSOFFSET 0x06
#define COMMANDSTATUS 0x07

#endif /* COMPACTFLASHINTERFACECARD_H_ */
