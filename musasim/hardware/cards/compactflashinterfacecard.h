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

#endif /* COMPACTFLASHINTERFACECARD_H_ */
