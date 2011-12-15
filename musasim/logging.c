/*
 * logging.c
 *
 *  Created on: Dec 15, 2011
 *      Author: daniel
 */

#include <stdarg.h>
#include <stdio.h>

#include "logging.h"

void log_println(int level, const char* tag, char * fmt, ...) {

	va_list argptr;
	va_start(argptr, fmt);
	printf("*** %s ***\t", tag);
	vprintf(fmt, argptr);
	printf("\n");
	va_end(argptr);

}
