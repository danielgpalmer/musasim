/*
 * timers.h
 *
 *  Created on: 22 Sep 2012
 *      Author: daniel
 */

#ifndef TIMERS_H_
#define TIMERS_H_

#define TIMERS_INTERRUPTFIRED_MATCHA (timers_timer_0_flags | 1)
#define TIMERS_INTERRUPTFIRED_MATCHB (timers_timer_0_flags | (1 << 1))
#define TIMERS_CLEARFLAGS (timers_timer_0_flags = 0x00)

void timers_setup(void);

#endif /* TIMERS_H_ */
