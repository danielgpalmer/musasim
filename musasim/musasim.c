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

bool shouldexit = false;

bool parseargs(int argc, char* argv[]) {

	if (argc == 2) {
		if (loadrom(argv[1])) {
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
		simreset();
		while (!shouldexit) {
			simstep();
			simtick();
			usleep(16);
		}
	}

}

void request_exit() {
	shouldexit = true;
}
