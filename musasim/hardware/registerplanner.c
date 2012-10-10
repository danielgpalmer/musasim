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

static void registerplanner_printblocks(requirements** registerblocks, bool traverse) {
	requirements** blocks = registerblocks;
	while ((*blocks)) {

		int regwidth = (*blocks)->registerwidth;
		int numregs = (*blocks)->numberofregisters;

		printf(
				"block -> register width %d, number of registers %d, total size %d, blockstart 0x%08"PRIx32", blockend 0x%08"PRIx32"\n",
				regwidth, numregs, (*blocks)->total, (*blocks)->blockstart, (*blocks)->blockend);

		if (traverse && (*blocks)->child != NULL ) {
			printf("---\n");
			registerplanner_printblocks((*blocks)->child, false);
			printf("---\n");
		}
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

		if (regwidth == 0) {
			int blocksize = 0;

			printf("---\n");

			requirements** children = (*blocks)->child;
			registerplanner_printblocks(children, false);
			while ((*children)) {
				blocksize += (*children)->total;
				(*blocks)->numberofregisters += (*children)->numberofregisters;
				children++;
			}

			if (blocksize > *biggest)
				*biggest = blocksize;

			(*blocks)->total = blocksize;

		}
		else {
			int numregs = (*blocks)->numberofregisters;
			int blocksize;

			// pad 8bit registers as they will only be connected to one byte lane
			if (regwidth == 1)
				blocksize = 2 * numregs;
			else if (regwidth == -1)
				blocksize = numregs;
			else {
				blocksize = (regwidth * numregs);
			}

			(*blocks)->total = blocksize;

			if (blocksize > *biggest)
				*biggest = blocksize;

			*total += blocksize;

		}
		blocks++;
		(*numberofblocks)++;
	}
}

void registerplanner_plan(requirements** registerblocks, scheme s, bool sort) {

	printf("planning registers\n");

	int numberofblocks = 0;
	int biggest = 0;
	int total = 0;

	registerplanner_prep(registerblocks, &biggest, &total, &numberofblocks);

	if (numberofblocks == 0)
		return;

	if (sort && numberofblocks > 1)
		registerplanner_sort(registerblocks);

	switch (s) {
		case chipselectblocks: {
			requirements** blocks = registerblocks;
			uint32_t blocksize = utils_nextpow(biggest);
			uint32_t last = 0;
			while ((*blocks)) {
				(*blocks)->blockstart = last;
				last += blocksize;
				(*blocks)->blockend = (*blocks)->blockstart + (*blocks)->total;

				requirements** children = (*blocks)->child;
				while ((*children)) {
					(*children)->blockstart += (*blocks)->blockstart;
					(*children)->blockend += (*blocks)->blockstart;
					children++;
				}

				blocks++;
			}
		}
			break;
		case packed: {
			requirements** blocks = registerblocks;
			uint32_t last = 0;
			while ((*blocks)) {
				(*blocks)->blockstart = last;
				last += (*blocks)->total + ((*blocks)->total % 4);
				(*blocks)->blockend = (*blocks)->blockstart + (*blocks)->total - 1;
				blocks++;
			}
		}
			break;
	}

	printf("register layout\n");
	registerplanner_printblocks(registerblocks, true);
}

int registerplanner_whichblock(requirements** blocks, uint32_t address) {

	int which = 0;

	while ((*blocks)) {

		if (address >= (*blocks)->blockstart && address <= (*blocks)->blockend)
			return which;

		which++;
		blocks++;
	}

	return -1;
}

int registerplanner_whichregister(requirements** peripheral, uint32_t address) {

	int which = 0;

	while ((*peripheral)) {

		if (address >= (*peripheral)->blockstart && address <= (*peripheral)->blockend) {
			return which;
		}

		which += (*peripheral)->numberofregisters;
		peripheral++;
	}

	return -1;
}
