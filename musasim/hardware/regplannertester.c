/*
 * regplannertester.c
 *
 *  Created on: 8 Oct 2012
 *      Author: daniel
 */

#include <stdio.h>
#include <stdbool.h>
#include "registerplanner.h"

int main() {

	printf("register planner tester\n");

	requirements peripheral0_0 = { 1, 1, 0, 0, 0, NULL };
	requirements peripheral0_1 = { 2, 6, 0, 0, 0, NULL };
	requirements peripheral0_2 = { 4, 20, 0, 0, 0, NULL };
	requirements peripheral0_3 = { 1, 2, 0, 0, 0, NULL };
	requirements peripheral0_4 = { 2, 4, 0, 0, 0, NULL };

	requirements* peripheral0[] =
			{ &peripheral0_0, &peripheral0_1, &peripheral0_2, &peripheral0_3, &peripheral0_4, NULL };

	registerplanner_plan(peripheral0, packed, false);

	requirements peripheral1_0 = { 1, 2, 0, 0, 0, NULL };
	requirements peripheral1_1 = { 2, 6, 0, 0, 0, NULL };
	requirements peripheral1_2 = { 4, 20, 0, 0, 0, NULL };
	requirements peripheral1_3 = { 1, 2, 0, 0, 0, NULL };
	requirements peripheral1_4 = { 2, 4, 0, 0, 0, NULL };

	requirements* peripheral1[] =
			{ &peripheral1_0, &peripheral1_1, &peripheral1_2, &peripheral1_3, &peripheral1_4, NULL };

	registerplanner_plan(peripheral1, packed, false);

	requirements peripheral2_0 = { 1, 3, 0, 0, 0, NULL };
	requirements peripheral2_1 = { 2, 6, 0, 0, 0, NULL };
	requirements peripheral2_2 = { 4, 20, 0, 0, 0, NULL };
	requirements peripheral2_3 = { 1, 2, 0, 0, 0, NULL };
	requirements peripheral2_4 = { 2, 4, 0, 0, 0, NULL };

	requirements* peripheral2[] =
			{ &peripheral2_0, &peripheral2_1, &peripheral2_2, &peripheral2_3, &peripheral2_4, NULL };

	registerplanner_plan(peripheral2, packed, false);

	requirements block0 = { 0, 0, 0, 0, 0, peripheral0 };
	requirements block1 = { 0, 0, 0, 0, 0, peripheral1 };
	requirements block2 = { 0, 0, 0, 0, 0, peripheral2 };
	requirements* blocks[] = { &block0, &block1, &block2, NULL };

	registerplanner_plan(blocks, chipselectblocks, true);

	return 0;
}
