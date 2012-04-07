/*
 * logging.c
 *
 *  Created on: Dec 15, 2011
 *      Author: daniel
 */

#include <stdarg.h>
#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include <stdbool.h>
#include <glib.h>

#include "logging.h"

#define LONGESTTAG 10

static int loglevel = LEVEL_DEBUG;

static const char YELLOW[] = "33";
static const char RED[] = "31";

static bool inited = false;
static GMutex* mutex = NULL;

static bool checkstate() {
	if (!inited) {
		printf("You must call log_init first!\n");
	}
	return inited;
}

#define CHECKSTATE if(!checkstate()){ return; }

void log_init() {
	mutex = g_mutex_new();
	inited = true;
}

void log_shutdown() {
	g_mutex_free(mutex);
	inited = false;
}

void log_println(int level, const char* tag, char * fmt, ...) {

	CHECKSTATE;

	if (level > loglevel) {
		return;
	}

	g_mutex_lock(mutex);

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

	g_mutex_unlock(mutex);

}

void log_setlevel(int newlevel) {
	CHECKSTATE;
	loglevel = newlevel;
}

void log_printhexblock(int level, const char* tag, void* data, size_t len) {

	CHECKSTATE;

	g_mutex_lock(mutex);

	int byte = 0;
	int row = 0;
	while (byte < len) {

		printf("0x%02x", row);

		// FIXME this is shit
		for (int i = 0; i < (LONGESTTAG); i++) {
			printf(" ");
		}

		for (int col = 0; col < 0xf; col++, byte++) {

			if (byte == len) {
				break;
			}

			uint8_t thisbyte = *((uint8_t*) (data + byte));

			if (thisbyte != 0) {
				printf("\033[1;40;%sm", RED);

			}

			char ascii = FILTERPRINTABLE(thisbyte);

			printf("0x%02x [%c] ", thisbyte, ascii);

			printf("\033[0m");

		}

		printf("\n");
		row++;
	}

	g_mutex_unlock(mutex);
}
