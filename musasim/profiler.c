/*
 * profiler.c
 *
 *  Created on: Mar 31, 2012
 *      Author: daniel
 */

#include <sys/gmon_out.h>
#include "profiler.h"
#include <stdbool.h>

static bool enabled = false;

void profiler_init() {
	enabled = true;
}

void profiler_onpcchange(uint32_t pc) {

	if (!enabled) {
		return;
	}

}

void profiler_flush() {

}
