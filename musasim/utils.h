/*
 * utils.h
 *
 *  Created on: Dec 18, 2011
 *      Author: daniel
 */

#ifndef UTILS_H_
#define UTILS_H_

#include <stdint.h>
#include <time.h>

uint32_t utils_nextpow(uint32_t value);
struct timespec* timespecdiff(struct timespec *start, struct timespec *end);

#endif /* UTILS_H_ */
