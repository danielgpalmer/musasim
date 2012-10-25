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

static int registerplanner_plangroup(registergroup* reggroup, int offset) {
	int stride = 1;

	if (reggroup->registerwidth == 1)
		stride = 2;
	else
		stride = reggroup->registerwidth;

	int alignment = offset % stride;
	reggroup->bytes = alignment + (stride * reggroup->numberofregisters);

	for (int i = 0; i < reggroup->numberofregisters; i++) {
		printf("register width %d, alignment %d, %d\n", reggroup->registerwidth, alignment,
				offset + alignment + (stride * i));
	}

	return offset += reggroup->bytes;
}

static int registerplanner_planperipheral(peripheral* peripheral) {
	peripheral->bytes = 0;
	registergroup** registergroups = peripheral->registergroups;
	while (*registergroups != NULL ) {
		peripheral->bytes = registerplanner_plangroup(*registergroups, peripheral->bytes);
		registergroups++;
	}
	printf("---\n");

	peripheral->peripheralstart = 0;
	peripheral->peripheralend = 0;
	return peripheral->bytes;
}

static void registerplanner_printregistergroup(registergroup* group) {
	printf("\\   register group,bytes %d\n", group->bytes);
	const char** names = group->registernames;
	if (names != NULL ) {
		while (*names != NULL ) {
			printf(" \\_ %s\n", *names);
			names++;
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
		((*units)->either).end = ((*units)->either).start + ((*units)->either).bytes;
		last += unitspacing;
		units++;
	}
}

unit* registerplanner_createperipheral(peripheral* template, module* module, void* context) {
	unit* unit = malloc(sizeof(unit));
	memcpy(unit, template, sizeof(unit));
	unit->type = PERIPHERAL;
	unit->peripheral.module = module;
	unit->peripheral.context = context;
	return unit;
}

unit* registerplanner_createblock(int size, void* backingarray) {
	unit* unit = malloc(sizeof(unit));
	unit->type = BLOCK;
	unit->block.bytes = size;
	unit->block.block = backingarray;
	return unit;
}
