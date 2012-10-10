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
	chipselectblocks, // use to plan blocks of peripheral register blocks
	packed // user to plan a block of registers for a peripheral
} scheme;

typedef struct requirements requirements;

struct requirements {
	int registerwidth; // how many byte wide the registers are. Must be 0,1, 2,4 or -1, 0 means that this is a container and the real registers are in the child, 8bit registers will be expanded to 16bits so that they only sit on one bytelane, to put 8bit registers on both lanes pass -1
	int numberofregisters; // how many registers you want
	int total; // the total number of bytes of address space that will be used, this is filled in by the planner
	uint32_t blockstart; // filled in by the planner
	uint32_t blockend; // filled in by the planner;
	requirements** child;
};

void registerplanner_plan(requirements** registerblocks, scheme s, bool sort);
int registerplanner_whichblock(requirements** blocks, uint32_t address);
int registerplanner_whichregister(requirements** peripheral, uint32_t address);

#endif /* REGISTERPLANNER_H_ */
