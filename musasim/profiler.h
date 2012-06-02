/**
 * \file profiler.h
 * \brief gprof output generator
 */

#ifndef PROFILER_H_
#define PROFILER_H_

#include <stdint.h>

void profiler_init(const char* filepath);
void profiler_onpcchange(uint32_t pc);
void profiler_onpcmodified(uint32_t a1, uint32_t a2);
void profiler_flush();

#endif /* PROFILER_H_ */
