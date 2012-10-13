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
#include "registerplanner.h"
#include "../utils.h"

static void registerplanner_preproup(registergroup* reggroup, int* total) {
	if (reggroup->registerwidth == -1)
		reggroup->bytes = reggroup->numberofregisters;
	else if (reggroup->registerwidth == 1)
		reggroup->bytes = reggroup->numberofregisters * 2;
	else
		reggroup->bytes = reggroup->numberofregisters * reggroup->registerwidth;

	*total += reggroup->bytes;
}

static int registerplanner_planperipheral(peripheral* peripheral) {
	peripheral->bytes = 0;
	registergroup** registergroups = peripheral->registergroups;
	while (*registergroups != NULL ) {
		registerplanner_preproup(*registergroups, &(peripheral->bytes));
		registergroups++;
	}

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
		((*units)->either).end = last + unitspacing - 1;
		last = last + unitspacing;
		units++;
	}
}

