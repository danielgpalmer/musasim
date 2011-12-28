/*
 * logging.h
 *
 *  Created on: Dec 15, 2011
 *      Author: daniel
 */

#ifndef LOGGING_H_
#define LOGGING_H_

#define LEVEL_INFO 0 // Normal output
#define LEVEL_WARNING 1 // Stderr type stuff
#define LEVEL_DEBUG 2 // Verbose debug stuff
#define LEVEL_INSANE 98 //
#define LEVEL_ALL 99
void log_println(int level, const char* tag, char * fmt, ...);
void log_setlevel(int newlevel);

void log_printhexblock(int level, const char* tag, void* data, size_t len);

#define FILTERPRINTABLE(c) ((c >= 0x20 && c <= 0x7F) ? c : ' ')

#endif /* LOGGING_H_ */
