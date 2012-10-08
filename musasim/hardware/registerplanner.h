/*
 * registerplanner.h
 *
 *  Created on: 8 Oct 2012
 *      Author: daniel
 */

#ifndef REGISTERPLANNER_H_
#define REGISTERPLANNER_H_

#include <stdint.h>

typedef enum {
	leastlogic //
} scheme;

typedef struct {
	int registerwidth; // how many byte wide the registers are. Must be 1, 2 or 4
	int numberofregisters; // how many registers you want
	int total; //
	uint32_t blockstart; // filled in by the planner
	uint32_t blockend; // filled in by the planner;
} requirements;

void registerplanner_plan(requirements** registerblocks, scheme s);

#endif /* REGISTERPLANNER_H_ */
