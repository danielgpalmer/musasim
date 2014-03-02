/*
 * args.c
 *
 *  Created on: Dec 27, 2011
 *      Author: daniel
 */

#include <argtable2.h>
#include "args.h"

#include <stdbool.h>
#include <stdio.h>

#include "hardware/cards/romcard.h"
#include "hardware/cards/compactflashinterfacecard.h"
#include "hardware/cards/uartcard.h"
#include "hardware/cards/soundcard.h"

#include "sim.h"
#include "logging.h"

#ifdef GDBSERVER
#include "gdbserver.h"
#include "profiler.h"
#endif

bool args_parse(int argc, char* argv[]) {

	struct arg_lit *help = arg_lit0(NULL, "help", "print this help and exit");
	struct arg_file *rompath = arg_file0("r", "rom", "binfile",
			"Path of the ROM binary");
	struct arg_file *cfpath = arg_file0("c", "compactflash", "binfile",
			"Path of the Compact Flash binary image");
	struct arg_file *elfpath = arg_file0("e", "elf", "elf executable",
			"Path of an Elf binary to load into ROM");
	struct arg_lit *loguartchone = arg_lit0(NULL, "loguartchanone",
			"Log bytes coming out of uart channel one to stdout");
	struct arg_lit *osd = arg_lit0(NULL, "osd", "Display the osd at startup");
	struct arg_lit *basicvideo = arg_lit0(NULL, "basicvideo",
			"Use the basic video card");
	struct arg_lit *basicsound = arg_lit0(NULL, "basicsound",
			"Use the basic sound card");
	struct arg_lit *disablesound = arg_lit0(NULL, "disablesound",
			"Disable the sound card");
	struct arg_int *loglevel = arg_int0(NULL, "loglevel", "level", "");
	struct arg_end *end = arg_end(20);

#ifdef GDBSERVER
	struct arg_int *gdbport = arg_int1("p", "port", "", "Port to listen for GDB connections");
	struct arg_file *profile = arg_file0("m", "profileroutput", "",
			"Trace program execution and write it to a gprof file");
	void *argtable[] = {help, rompath, elfpath, cfpath, gdbport, profile, loguartchone, osd, loglevel, basicvideo,
		basicsound, disablesound, end};
#else
	void *argtable[] = { help, rompath, elfpath, cfpath, loguartchone, osd,
			basicvideo, loglevel, basicsound, disablesound, end };
#endif

	if (arg_nullcheck(argtable) != 0) {
		/* NULL entries were detected, some allocations must have failed */
		printf("%s: insufficient memory\n", argv[0]);
		return false;
	}

	int nerrors = arg_parse(argc, argv, argtable);

	if (help->count > 0) {
		printf("Usage: %s", argv[0]);
		arg_print_syntax(stdout, argtable, "\n");
		arg_print_glossary(stdout, argtable, "  %-30s %s\n");
		return false;
	}

	/* If the parser returned any errors then display them and exit */
	else if (nerrors > 0) {
		/* Display the error details contained in the arg_end struct.*/
		arg_print_errors(stdout, end, argv[0]);
		printf("Try '%s --help' for more information.\n", argv[0]);
		return false;
	}

	else {

#ifndef GDBSERVER
		if (rompath->count == 0 && elfpath->count == 0) {
			printf("You must pass either a rom or elf to load\n");
			return false;
		}
#endif

		if (rompath->count > 0 && elfpath->count > 0) {
			printf("You can either load a rom or an elf, not both\n");
			return false;
		}

		if (rompath->count == 1) {
			if (!romcard_loadrom(*(rompath->filename), false)) {
				return false;
			}
		} else if (elfpath->count == 1) {
			if (!romcard_loadrom(*(elfpath->filename), true)) {
				return false;
			}
		}

		if (cfpath->count > 0) {
			if (!cfintf_load(*(cfpath->filename))) {
				return false;
			}
		}

#ifdef GDBSERVER
		gdbserver_setport(*(gdbport->ival));

		if (profile->count == 1) {
			printf("enabling profiler\n");
			profiler_init(*(profile->filename));
		}

#endif

		sim_setoptions(basicvideo->count == 1, basicsound->count == 1,
				osd->count == 1);

		if (disablesound->count == 1)
			soundcard_disable(true);

		if (loguartchone->count == 1) {
			uart_enable_logging();
		}

		if (loglevel->count == 1) {
			log_setlevel(*(loglevel->ival));
		}

		arg_freetable(argtable, sizeof(argtable) / sizeof(argtable[0]));

	}

	return true;

}
