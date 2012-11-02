/*
 * registerplanner.c
 *
 *  Created on: 8 Oct 2012
 *      Author: daniel
 */

#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>
#include <inttypes.h>
#include <string.h>
#include "registerplanner.h"
#include "../utils.h"

static int registerplanner_widthtostride(int width) {
	int stride = 1;

	if (width == 1)
		stride = 2;
	else
		stride = width;

	return stride;
}

static int registerplanner_widthtosize(int width) {
	int w = 0;

	if (w == -1)
		w = 1;
	else
		w = width;

	return w;
}

static int registerplanner_plangroup(registergroup* reggroup, int offset) {

	int stride = registerplanner_widthtostride(reggroup->registerwidth);

	int alignment = offset % stride;
	reggroup->bytes = alignment + (stride * reggroup->numberofregisters);

	reggroup->groupstart = offset;
	reggroup->groupend = offset + reggroup->bytes - reggroup->registerwidth;

	return offset += reggroup->bytes;
}

static int registerplanner_planperipheral(peripheral* peripheral) {
	peripheral->bytes = 0;
	registergroup** registergroups = peripheral->registergroups;
	while (*registergroups != NULL ) {
		peripheral->bytes = registerplanner_plangroup(*registergroups, peripheral->bytes);
		registergroups++;
	}

	peripheral->peripheralstart = 0;
	peripheral->peripheralend = 0;
	return peripheral->bytes;
}

static void registerplanner_printregistergroup(registergroup* group) {
	printf("\\   0x%08"PRIx32" -> 0x%08"PRIx32" - register group,bytes %d\n", group->groupstart, group->groupend,
			group->bytes);
	const char** names = group->registernames;
	if (names != NULL ) {
		uint32_t registeraddress = group->groupstart;
		while (*names != NULL ) {
			printf(" \\_ 0x%08"PRIx32" - %s\n", registeraddress, *names);
			names++;
			registeraddress += group->registerwidth;
		}
	}
}

static void registerplanner_printperipheral(peripheral* peripheral) {
	printf("0x%08"PRIx32" -> 0x%08"PRIx32 " - peripheral, %d bytes\n", peripheral->peripheralstart,
			peripheral->peripheralend, peripheral->bytes);
	registergroup** registergroups = peripheral->registergroups;
	while (*registergroups != NULL ) {
		registerplanner_printregistergroup(*registergroups);
		registergroups++;
	}
}

static int registerplanner_planblock(block* block) {
	block->blockstart = 0;
	block->blockend = 0;
	return block->bytes;
}

static void registerplanner_printblock(block* block) {
	printf("0x%08"PRIx32" -> 0x%08"PRIx32 " - block, %d bytes\n", block->blockstart, block->blockend, block->bytes);
}

void registerplanner_print(cardaddressspace* card) {
	unit** units = card->units;
	while (*units != NULL ) {
		switch ((*units)->type) {
			case BLOCK:
				registerplanner_printblock(&(*units)->block);
				break;
			case PERIPHERAL:
				registerplanner_printperipheral(&(*units)->peripheral);
				break;
		}
		units++;
	}
}

void registerplanner_plan(cardaddressspace* card) {
	unit** units = card->units;
	int biggestunit = 0;
	while (*units != NULL ) {
		int unitsize = 0;
		switch ((*units)->type) {
			case BLOCK:
				unitsize = registerplanner_planblock(&(*units)->block);
				break;
			case PERIPHERAL:
				unitsize = registerplanner_planperipheral(&(*units)->peripheral);
				break;
		}

		if (unitsize > biggestunit)
			biggestunit = unitsize;

		units++;
	}

	units = card->units;
	int unitspacing = utils_nextpow(biggestunit);
	uint32_t last = 0;
	while (*units != NULL ) {
		((*units)->either).start = last;
		((*units)->either).end = ((*units)->either).start + ((*units)->either).bytes - 1;
		last += unitspacing;
		units++;
	}
}

unit* registerplanner_createperipheral(const peripheral* template, char* name, const module* module, void* context) {
	int unitsize = sizeof(unit);
	unit* unit = malloc(unitsize);
	memcpy(unit, template, sizeof(peripheral));
	unit->type = PERIPHERAL;

	if (name != NULL )
		unit->either.name = name;

	unit->peripheral.module = module;
	unit->peripheral.context = context;
	return unit;
}

unit* registerplanner_createblock(int size, void* backingarray) {
	int unitsize = sizeof(unit);
	unit* unit = malloc(unitsize);
	unit->type = BLOCK;
	unit->block.bytes = size;
	unit->block.block = backingarray;
	return unit;
}

static unit* registerplanner_whichunit(cardaddressspace* card, uint32_t address) {
	printf("Looking for unit for 0x%"PRIx32"\n", address);
	unit** unit = card->units;
	while (*unit != NULL ) {
		if (address >= (*unit)->either.start && address <= (*unit)->either.end)
			return *unit;
		unit++;
	}
	return NULL ;
}

uint8_t registerplanner_read_byte(cardaddressspace* card, uint32_t address) {
	return 0;
}
uint16_t registerplanner_read_word(cardaddressspace* card, uint32_t address) {
	unit* unit = registerplanner_whichunit(card, address);
	if (unit != NULL ) {
		switch (unit->either.type) {
			case PERIPHERAL:
				return unit->peripheral.module->read_word(unit->peripheral.context, address);
			case BLOCK:
				return *((uint16_t*) unit->block.block); // hack
		}
	}
	return 0;
}
uint32_t registerplanner_read_long(cardaddressspace* card, uint32_t address) {
	return 0;
}
void registerplanner_write_byte(cardaddressspace* card, uint32_t address, uint8_t value) {

}
void registerplanner_write_word(cardaddressspace* card, uint32_t address, uint16_t value) {
	unit* unit = registerplanner_whichunit(card, address);
	if (unit != NULL ) {
		switch (unit->either.type) {
			case PERIPHERAL:
				unit->peripheral.module->write_word(unit->peripheral.context, address, value);
				break;
			case BLOCK:
				break;
		}
	}
}
void registerplanner_write_long(cardaddressspace* card, uint32_t address, uint32_t value) {

}

void registerplanner_iterate_registers(unit* unit,
		void (*function)(uint32_t address, int width, const char* name, void* data), void* data) {

	if (unit->type == PERIPHERAL) {
		for (registergroup** rg = unit->peripheral.registergroups; *rg != NULL ; rg++) {
			for (int r = 0; r < (*rg)->numberofregisters; r++) {
				uint32_t address = unit->either.start + (*rg)->groupstart
						+ (registerplanner_widthtostride((*rg)->registerwidth) * r);

				int size = registerplanner_widthtosize((*rg)->registerwidth);
				const char* name = (*rg)->registernames[r];

				function(address, size, name, data);
			}
		}
	}
}

void registerplanner_iterate(cardaddressspace* card, void (*function)(unit*)) {
	unit** unit = card->units;
	for (; *unit != NULL ; unit++) {
		function(*unit);
	}
}

void registerplanner_tickmodules(cardaddressspace* card, int cyclesexecuted) {
	unit** currentunit = card->units;
	while (*currentunit != NULL ) {
		unit* u = *currentunit;
		if (u->either.type == PERIPHERAL) {
			u->peripheral.module->tick(u->peripheral.context, cyclesexecuted);
		}
		currentunit++;
	}
}
