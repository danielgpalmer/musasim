#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <stdint.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>
#include <glib.h>

#include "sim.h"
#include "m68k.h"

#include "hardware/cards/romcard.h"

bool parseargs(int argc, char* argv[]) {

	if (argc == 2) {
		if (romcard_loadrom(argv[1])) {
			return true;
		}
	}

	else {
		printf("usage: %s [rom binary]\n", argv[0]);
	}

	return false;
}

int main(int argc, char* argv[]) {
	if (parseargs(argc, argv)) {
		sim_init();
		sim_reset();
		while (!sim_has_quit()) {
			sim_tick();
		}
	}
}
