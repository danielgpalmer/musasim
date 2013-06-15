/*
 * logging.c
 *
 *  Created on: Dec 15, 2011
 *      Author: daniel
 */

#include <stdlib.h>
#include <stdarg.h>
#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include <stdbool.h>
#include <glib.h>

#include "logging.h"

#define BUFFERSIZE 512

#ifdef GDBSERVER
static int loglevel = LEVEL_DEBUG;
#else
static int loglevel = LEVEL_INFO;
#endif

static const char YELLOW[] = "33";
static const char RED[] = "31";

static bool inited = false;
static GMutex mutex;

static char* buffer;

static bool checkstate() {
	if (!inited) {
		printf("You must call log_init first!\n");
	}
	return inited;
}

#define CHECKSTATE() if(!checkstate()){ return; }

void log_init() {
	buffer = malloc(BUFFERSIZE);
	inited = true;
}

void log_shutdown() {
	free(buffer);
	inited = false;
}

void log_println(int level, const char* tag, char * fmt, ...) {

	// level filtering
	if (level > loglevel) {
		return;
	}
	CHECKSTATE();

	g_mutex_lock(&mutex);

	switch (level) {
		case LEVEL_WARNING:
			printf("\033[1;40;%sm", RED);
			break;
		case LEVEL_DEBUG:
			printf("\033[1;40;%sm", YELLOW);
			break;
		default:
			printf("\033[1m");
			break;
	}

	snprintf(buffer, BUFFERSIZE, "[%s]", tag);
	printf("%-10s" "\033[0m", buffer);

	va_list argptr;
	va_start(argptr, fmt);
	vsnprintf(buffer, BUFFERSIZE, fmt, argptr);
	printf("%s\n", buffer);
	va_end(argptr);

	g_mutex_unlock(&mutex);

}

void log_setlevel(int newlevel) {
	CHECKSTATE();
	loglevel = newlevel;
}

void log_printhexblock(int level, const char* tag, void* data, size_t len) {

	if (level > loglevel) {
		return;
	}
	CHECKSTATE();

	g_mutex_lock(&mutex);

	int byte = 0;
	int row = 0;
	while (byte < len) {

		snprintf(buffer, BUFFERSIZE, "0x%02x", row);
		printf("%-10s", buffer);

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

	g_mutex_unlock(&mutex);
}
