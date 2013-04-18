/*
 * throttler.h
 *
 *  Created on: 18 Apr 2013
 *      Author: daniel
 */

#ifndef THROTTLER_H_
#define THROTTLER_H_

#include <stdbool.h>

void throttler_starttick();

void throttler_endtick();

void throttler_enable(bool enabled);

void throttler_toggle();

bool throttler_behind();

double throttler_speed();

#endif /* THROTTLER_H_ */
