#include "sim.h"
#include "args.h"
#include "logging.h"

int main(int argc, char* argv[]) {

	log_init();

	int ret = 0;

	if (args_parse(argc, argv)) {
		if (sim_init()) {
			sim_reset();
			while (!sim_has_quit()) {
				sim_tick();
			}
		} else {
			log_println(LEVEL_INFO, "main", "sim init failed, exiting\n");
			ret = 1;
		}

	}

	return ret;
}
