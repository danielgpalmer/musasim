/*
 * profiler.h
 *
 *  Created on: Mar 31, 2012
 *      Author: daniel
 */

#ifndef PROFILER_H_
#define PROFILER_H_

#include <stdint.h>

void profiler_init();
void profiler_onpcchange(uint32_t pc);
void profiler_flush();

#endif /* PROFILER_H_ */
