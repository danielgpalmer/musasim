#include <stdio.h>
#include <argtable2.h>

#include "sim.h"
#include "m68k.h"
#include "logging.h"
#include "args.h"



int main(int argc, char* argv[]) {

	if (args_parse(argc, argv)) {
		sim_init();
		sim_reset();
		while (!sim_has_quit()) {
			sim_tick();
		}
	}
}
