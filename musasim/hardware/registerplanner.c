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

static void registerplanner_printblocks(requirements** registerblocks) {
	requirements** blocks = registerblocks;
	while ((*blocks)) {

		int regwidth = (*blocks)->registerwidth;
		int numregs = (*blocks)->numberofregisters;
		int blocksize = regwidth * numregs;
		(*blocks)->total = blocksize;

		printf(
				"block -> register width %d, number of registers %d, total size %d, blockstart 0x%08"PRIx32", blockend 0x%08"PRIx32"\n",
				regwidth, numregs, blocksize, (*blocks)->blockstart, (*blocks)->blockend);
		blocks++;

	}
}

static bool registerplanner_sortinner(requirements** registerblocks) {

	bool clean = true;

	requirements** blocks = registerblocks;
	while ((*blocks)) {

		requirements** thisblock = blocks;
		requirements** nextblock = blocks + 1;
		requirements* tmp;

		if ((*nextblock) != NULL ) {
			if ((*nextblock)->total < (*thisblock)->total) {
				clean = false;
				tmp = *thisblock;
				*thisblock = *nextblock;
				*nextblock = tmp;
			}
		}
		blocks++;

	}

	return clean;

}

static void registerplanner_sort(requirements** registerblocks) {
	while (!registerplanner_sortinner(registerblocks))
		;
}

static void registerplanner_prep(requirements** registerblocks, int* biggest, int* total, int* numberofblocks) {
	requirements** blocks = registerblocks;
	while ((*blocks)) {
		int regwidth = (*blocks)->registerwidth;
		int numregs = (*blocks)->numberofregisters;
		int blocksize = regwidth * numregs;
		(*blocks)->total = blocksize;

		if (blocksize > *biggest)
			*biggest = blocksize;

		*total += blocksize;

		blocks++;
		*numberofblocks = *numberofblocks + 1;
	}
}

void registerplanner_plan(requirements** registerblocks, scheme s) {

	printf("planning registers\n");

	int numberofblocks = 0;
	int biggest = 0;
	int total = 0;

	registerplanner_prep(registerblocks, &biggest, &total, &numberofblocks);

	if (numberofblocks == 0)
		return;

	if (numberofblocks > 1)
		registerplanner_sort(registerblocks);

	requirements** blocks = registerblocks;
	uint32_t blocksize = utils_nextpow(biggest);
	uint32_t last = 0;
	while ((*blocks)) {
		(*blocks)->blockstart = last;
		last += blocksize;
		(*blocks)->blockend = (*blocks)->blockstart + (*blocks)->total;
		blocks++;
	}

	printf("register layout\n");
	registerplanner_printblocks(registerblocks);
}
