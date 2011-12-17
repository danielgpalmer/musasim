/*
 * logging.c
 *
 *  Created on: Dec 15, 2011
 *      Author: daniel
 */

#include <stdarg.h>
#include <stdio.h>

#include "logging.h"

static int loglevel = LEVEL_ALL;

static const char YELLOW[] = "33";
static const char RED[] = "31";

void log_println(int level, const char* tag, char * fmt, ...) {

	if (level > loglevel) {
		return;
	}

	va_list argptr;
	va_start(argptr, fmt);

	if (level == LEVEL_WARNING) {
		printf("\033[0;40;%sm", RED);
	}

	if (level == LEVEL_DEBUG) {
		printf("\033[0;40;%sm", YELLOW);
	}

	printf("[%s]\t", tag);
	printf("\033[0m");
	vprintf(fmt, argptr);
	printf("\n");
	va_end(argptr);

}

void log_setlevel(int newlevel) {
	loglevel = newlevel;
}
