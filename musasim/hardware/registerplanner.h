/*
 * registerplanner.h
 *
 *  Created on: 8 Oct 2012
 *      Author: daniel
 */

#ifndef REGISTERPLANNER_H_
#define REGISTERPLANNER_H_

#include <stdint.h>
#include "modules/module.h"

typedef enum {
	PERIPHERAL, BLOCK
} unittype;

typedef struct peripheral peripheral;
typedef struct block block;
typedef struct either either;
typedef struct registergroup registergroup;

struct either {
	unittype type;
	int bytes;
	uint32_t start;
	uint32_t end;
	char* name;
};

struct peripheral {
	unittype type;
	int bytes;
	uint32_t peripheralstart;
	uint32_t peripheralend;
	char* name;
	//
	int numberofregistergroups;
	registergroup** registergroups;
	const module* module;
	void* context;
};

struct block {
	unittype type;
	int bytes;
	uint32_t blockstart;
	uint32_t blockend;
	char* name;
	//
	void* block;
};

typedef union {
	unittype type;
	either either;
	peripheral peripheral;
	block block;
} unit;

typedef struct {
	int numberofperipherals;
	unit** units;
} cardaddressspace;

struct registergroup {
// provide to the planner
	int registerwidth; // how many bytes wide the registers are. Must be 1, 2,4 or -1, 8bit registers will be expanded to 16bits so that they only sit on one bytelane, to put 8bit registers on both lanes pass -1
	int numberofregisters; // how many registers you want
	const char** registernames;
// filler in by the planner
	uint32_t groupstart; // filled in by the planner
	uint32_t groupend; // filled in by the planner;
	int bytes; // the total number of bytes of address space that will be used, this is filled in by the planner
};

void registerplanner_plan(cardaddressspace* card);
void registerplanner_print(cardaddressspace* card);
unit* registerplanner_createperipheral(const peripheral* template, char* name, const module* module, void* context);
unit* registerplanner_createblock(int bytes, void* backingarray);

uint8_t registerplanner_read_byte(cardaddressspace* card, uint32_t address);
uint16_t registerplanner_read_word(cardaddressspace* card, uint32_t address);
uint32_t registerplanner_read_long(cardaddressspace* card, uint32_t address);
void registerplanner_write_byte(cardaddressspace* card, uint32_t address, uint8_t value);
void registerplanner_write_word(cardaddressspace* card, uint32_t address, uint16_t value);
void registerplanner_write_long(cardaddressspace* card, uint32_t address, uint32_t value);

void registerplanner_iterate(cardaddressspace* card, void (*function)(unit*));
void registerplanner_iterate_registers(unit* unit,
		void (*function)(uint32_t address, int width, const char* name, void* data), void* data);
void registerplanner_tickmodules(cardaddressspace* card, int cyclesexecuted);

#endif /* REGISTERPLANNER_H_ */
