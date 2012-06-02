/**
 *	\file logging.h
 *	\brief standard logging functions
 */

#ifndef LOGGING_H_
#define LOGGING_H_

#include <stddef.h>

#define LEVEL_INFO 0 // Normal output
#define LEVEL_WARNING 1 // Stderr type stuff
#define LEVEL_DEBUG 2 // Verbose debug stuff
#define LEVEL_INSANE 98 //
#define LEVEL_ALL 99

/**
 * init the logging, call this before anything else
 */

void log_init(void);

/**
 * works like printf
 */

void log_println(int level, const char* tag, char * fmt, ...) __attribute__ ((format (printf, 3, 4)));

/**
 * sets the logging level, anything below the set level will be filtered
 */


void log_setlevel(int newlevel);

/**
 * prints a block of data as a formatted block of hex digits
 */

void log_printhexblock(int level, const char* tag, void* data, size_t len);

/**
 * shutdown logging and free any resources that were allocated
 */

void log_shutdown(void);

#define FILTERPRINTABLE(c) ((c >= 0x20 && c <= 0x7F) ? c : ' ')

#endif /* LOGGING_H_ */
