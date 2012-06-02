#include "sim.h"
#include "args.h"
#include "logging.h"

int main(int argc, char* argv[]) {

	log_init();

	if (args_parse(argc, argv)) {
		sim_init();
		sim_reset();
		while (!sim_has_quit()) {
			sim_tick();
		}
	}

	return 0;
}
