/*
 * regplannertester.c
 *
 *  Created on: 8 Oct 2012
 *      Author: daniel
 */

#include <stdio.h>
#include "registerplanner.h"

int main() {

	printf("register planner tester\n");

	requirements block0 = { 1, 15, 0, 0, 0 };
	requirements block1 = { 2, 6, 0, 0, 0 };
	requirements block2 = { 4, 20, 0, 0, 0 };
	requirements block3 = { 1, 2, 0, 0, 0 };
	requirements block4 = { 2, 4, 0, 0, 0 };

	requirements* blocks[] = { &block0, &block1, &block2, &block3, &block4, NULL };

	registerplanner_plan(blocks, leastlogic);

	return 0;
}
