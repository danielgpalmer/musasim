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

static void registerplanner_plangroup(registergroup* group) {

}

void registerplanner_plan(cardaddressspace* card) {

	peripheral** peripherals = card->peripherals;
	while (*peripherals != NULL ) {
		registergroup** registergroups = (*peripherals)->registergroups;
		while (*registergroups != NULL ) {
			registerplanner_plangroup(*registergroups);
			if ((*registergroups)->registernames != NULL ) {
				const char** registernames = (*registergroups)->registernames;
				while (*registernames != NULL ) {
					printf("%s\n", *registernames);
					registernames++;
				}
			}
			registergroups++;
		}
		peripherals++;
	}

}

