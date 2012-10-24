/*
 * regplannertester.c
 *
 *  Created on: 8 Oct 2012
 *      Author: daniel
 */

#include <stdio.h>
#include <stdbool.h>
#include "registerplanner.h"
#include "../utils.h"

int main() {

	printf("register planner tester\n");

	// peripheral 0
	const char* peripheral0_0_names[] = { "dummyregisters_0_0", NULL };
	const char* peripheral0_1_names[] = { "dummyregisters_0_1", "dummyregisters_0_2", "dummyregisters_0_3",
			"dummyregisters_0_4", "dummyregisters_0_5", "dummyregisters_0_6", NULL };
	const char* peripheral0_2_names[] = { "dummyregisters_0_7", "dummyregisters_0_8", "dummyregisters_0_9", NULL };
	const char* peripheral0_3_names[] = { "dummyregisters_0_10", "dummyregisters_0_11", "dummyregisters_0_12", NULL };
	const char* peripheral0_4_names[] = { "dummyregisters_0_13", "dummyregisters_0_14", "dummyregisters_0_15",
			"dummyregisters_0_16", NULL };

	registergroup peripheral0_0 = { 1, 1, peripheral0_0_names, 0, 0, 0 };
	registergroup peripheral0_1 = { 2, 6, peripheral0_1_names, 0, 0, 0 };
	registergroup peripheral0_2 = { 4, 3, peripheral0_2_names, 0, 0, 0 };
	registergroup peripheral0_3 = { 1, 2, peripheral0_3_names, 0, 0, 0 };
	registergroup peripheral0_4 = { 2, 4, peripheral0_4_names, 0, 0, 0 };

	registergroup* peripheral0groups[] = { &peripheral0_0, &peripheral0_1, &peripheral0_2, &peripheral0_3,
			&peripheral0_4, NULL };

	unit peripheral0;
	peripheral0.type = PERIPHERAL;
	peripheral0.peripheral.registergroups = peripheral0groups;
	peripheral0.peripheral.numberofregistergroups = SIZEOFARRAY(peripheral0groups) - 1;
	//

	// peripheral 1
	const char* peripheral1_0_names[] = { "dummyregisters_1_0", NULL };
	const char* peripheral1_1_names[] = { "dummyregisters_1_1", "dummyregisters_1_2", "dummyregisters_1_3",
			"dummyregisters_1_4", "dummyregisters_1_5", "dummyregisters_1_6", NULL };
	const char* peripheral1_2_names[] = { "dummyregisters_1_7", NULL };
	const char* peripheral1_3_names[] = { "dummyregisters_1_8", "dummyregisters_1_9", NULL };
	const char* peripheral1_4_names[] = { "dummyregisters_1_10", "dummyregisters_1_11", "dummyregisters_1_12",
			"dummyregisters_1_13", NULL };

	registergroup peripheral1_0 = { 1, 2, peripheral1_0_names, 0, 0, 0 };
	registergroup peripheral1_1 = { 2, 6, peripheral1_1_names, 0, 0, 0 };
	registergroup peripheral1_2 = { 4, 1, peripheral1_2_names, 0, 0, 0 };
	registergroup peripheral1_3 = { 1, 2, peripheral1_3_names, 0, 0, 0 };
	registergroup peripheral1_4 = { 2, 4, peripheral1_4_names, 0, 0, 0 };

	registergroup* peripheral1groups[] = { &peripheral1_0, &peripheral1_1, &peripheral1_2, &peripheral1_3,
			&peripheral1_4, NULL };

	unit peripheral1;
	peripheral1.type = PERIPHERAL;
	peripheral1.peripheral.registergroups = peripheral1groups;
	peripheral1.peripheral.numberofregistergroups = SIZEOFARRAY(peripheral1groups) - 1;
	//

	// peripheral 2
	const char* peripheral2_0_names[] = { "dummyregisters_2_0", "dummyregisters_2_1", "dummyregisters_2_2", NULL };
	const char* peripheral2_1_names[] = { "dummyregisters_1_3", "dummyregisters_1_4", "dummyregisters_1_5",
			"dummyregisters_1_6", "dummyregisters_1_7", "dummyregisters_1_8", NULL };
	const char* peripheral2_2_names[] = { "dummyregisters_1_9", "dummyregisters_1_10", "dummyregisters_1_11",
			"dummyregisters_1_12", NULL };
	const char* peripheral2_3_names[] = { "dummyregisters_1_13", "dummyregisters_1_14", NULL };
	const char* peripheral2_4_names[] = { "dummyregisters_1_15", "dummyregisters_1_16", "dummyregisters_1_17",
			"dummyregisters_1_18", NULL };

	registergroup peripheral2_0 = { 1, 3, peripheral2_0_names, 0, 0, 0 };
	registergroup peripheral2_1 = { 2, 6, peripheral2_1_names, 0, 0, 0 };
	registergroup peripheral2_2 = { 4, 4, peripheral2_2_names, 0, 0, 0 };
	registergroup peripheral2_3 = { 1, 2, peripheral2_3_names, 0, 0, 0 };
	registergroup peripheral2_4 = { 2, 4, peripheral2_4_names, 0, 0, 0 };

	registergroup* peripheral2groups[] = { &peripheral2_0, &peripheral2_1, &peripheral2_2, &peripheral2_3,
			&peripheral2_4, NULL };

	unit peripheral2;
	peripheral2.type = PERIPHERAL;
	peripheral2.peripheral.registergroups = peripheral2groups;
	peripheral2.peripheral.numberofregistergroups = SIZEOFARRAY(peripheral2groups) - 1;
	//

	// a block of memory
	unit block0;
	block0.type = BLOCK;
	block0.block.bytes = 1056;
	//

	unit* cardunits[] = { &peripheral0, &peripheral1, &peripheral2, &block0, NULL };
	cardaddressspace card;
	card.units = cardunits;
	card.numberofperipherals = SIZEOFARRAY(cardunits) - 1;

	registerplanner_plan(&card);
	registerplanner_print(&card);

	//uint32_t registeraddress = 0x10a;
	//int block = registerplanner_whichblock(card, registeraddress);
	//int regnum = registerplanner_whichregister(blocks[block]->child, registeraddress);

	//printf("in block %d, register %d \n", block, regnum);

	return 0;
}
