/*
 * timers.h
 *
 *  Created on: 22 Sep 2012
 *      Author: daniel
 */

#ifndef TIMERS_H_
#define TIMERS_H_

#include <stdint.h>

void timers_setup(int which, uint16_t config, uint16_t prescaler, uint16_t matcha, uint16_t matchb);
void timers_bigsetup(int which, uint16_t config, uint32_t prescaler, uint32_t matcha, uint32_t matchb);

#endif /* TIMERS_H_ */
