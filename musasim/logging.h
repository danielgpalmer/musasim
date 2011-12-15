/*
 * logging.h
 *
 *  Created on: Dec 15, 2011
 *      Author: daniel
 */

#ifndef LOGGING_H_
#define LOGGING_H_

#define LEVEL_INFO 0
#define LEVEL_DEBUG 1

void log_println(int level, const char* tag, char * fmt, ...);

#endif /* LOGGING_H_ */
