/*
 * logging.c
 *
 *  Created on: Dec 15, 2011
 *      Author: daniel
 */

#include <stdarg.h>
#include <stdio.h>
#include <string.h>

#include "logging.h"

#define LONGESTTAG 10

static int loglevel = LEVEL_DEBUG;

static const char YELLOW[] = "33";
static const char RED[] = "31";

void log_println(int level, const char* tag, char * fmt, ...) {

	if (level > loglevel) {
		return;
	}

	int taglen = strlen(tag);

	va_list argptr;
	va_start(argptr, fmt);

	if (level == LEVEL_WARNING) {
		printf("\033[1;40;%sm", RED);
	}

	else if (level == LEVEL_DEBUG) {
		printf("\033[1;40;%sm", YELLOW);
	}

	else {
		printf("\033[1m");
	}

	printf("[%s]", tag);

	// FIXME this is shit
	for (int i = 0; i < (LONGESTTAG - taglen); i++) {
		printf(" ");
	}

	printf("\033[0m");
	vprintf(fmt, argptr);
	printf("\n");
	va_end(argptr);

}

void log_setlevel(int newlevel) {
	loglevel = newlevel;
}
