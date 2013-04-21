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

void throttler_startcputick();

void throttler_endcputick();

void throttler_startcardtick(int slot);

void throttler_endcardtick(int slot);

void throttler_endtick();

void throttler_enable(bool enabled);

void throttler_toggle();

bool throttler_behind();

double throttler_speed();

double throttler_overhead();

#endif /* THROTTLER_H_ */
