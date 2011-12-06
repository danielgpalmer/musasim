#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <stdint.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>
#include <glib.h>

#include "common.h"
#include "sim.h"
#include "m68k.h"

#include "hardware/cards/romcard.h"

bool shouldexit = false;

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
		while (!shouldexit) {
			sim_tick();
			usleep(16);
		}
	}

}

void request_exit() {
	shouldexit = true;
}
