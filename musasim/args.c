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

#ifdef GDBSERVER
#include "gdbserver.h"
#endif

bool args_parse(int argc, char* argv[]) {

	struct arg_lit *help = arg_lit0(NULL, "help", "print this help and exit");
	struct arg_file *rompath = arg_file1("r", "rom", "binfile", "Path of the ROM binary");
	struct arg_file *cfpath = arg_file0("c", "compactflash", "binfile", "Path of the Compact Flash binary image");
	struct arg_end *end = arg_end(20);

#ifdef GDBSERVER
	struct arg_int *gdbport = arg_int1("p", "port", "", "Port to listen for GDB connections");
	void *argtable[] = { help, rompath, cfpath, gdbport, end };
#else
	void *argtable[] = {help, rompath, cfpath, end};
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
		if (!romcard_loadrom(*(rompath->filename))) {
			return false;
		}
		if (cfpath->count > 0) {
			if (!cfintf_load(*(cfpath->filename))) {
				return false;
			}
		}

#ifdef GDBSERVER
		gdbserver_setport(*(gdbport->ival));
#endif

		arg_freetable(argtable, sizeof(argtable) / sizeof(argtable[0]));

	}

	return true;

}
