/*
 * utils.c
 *
 *  Created on: Dec 18, 2011
 *      Author: daniel
 */

#define _POSIX_C_SOURCE 1993309

#include "utils.h"
#include <stdint.h>
#include <stddef.h>
#include "musashi/m68k.h"
#include <time.h>
#include <stdio.h>

uint32_t utils_nextpow(uint32_t value) {
	value--;
	value |= value >> 1;
	value |= value >> 2;
	value |= value >> 4;
	value |= value >> 8;
	value |= value >> 16;
	value++;
	return value;
}

struct timespec* timespecdiff(struct timespec *start, struct timespec *end) {
	static struct timespec temp;
	if ((end->tv_nsec - start->tv_nsec) < 0) {
		temp.tv_sec = end->tv_sec - start->tv_sec - 1;
		temp.tv_nsec = 1000000000 + end->tv_nsec - start->tv_nsec;
	}
	else {
		temp.tv_sec = end->tv_sec - start->tv_sec;
		temp.tv_nsec = end->tv_nsec - start->tv_nsec;
	}
	return &temp;
}


