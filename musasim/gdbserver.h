/*
 * gdbserver.h
 *
 *  Created on: Jan 11, 2012
 *      Author: daniel
 */

#ifndef GDBSERVER_H_
#define GDBSERVER_H_

void gdbserver_check_breakpoints();
void gdbserver_setport(int p);
void gdb_hitstop();

#endif /* GDBSERVER_H_ */
