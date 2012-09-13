/*
 * gdbserver_private.h
 *
 *  Created on: 14 Sep 2012
 *      Author: daniel
 */

#ifndef GDBSERVER_PRIVATE_H_
#define GDBSERVER_PRIVATE_H_

#include <stdbool.h>

static const char TAG[] = "gdbserver";

static void gdbserver_check_breakpoints(uint32_t pc);

typedef struct {
	uint32_t address;
	unsigned int length;
} watchpoint;

typedef enum State {
	INIT, LISTENING, WAITING, RUNNING, STEPPING, BREAKING, EXIT

} State;

static void gdbserver_mainloop(void);

// stuff that pokes the sim
static char* gdbserver_readmem(char* commandbuffer);
static void gdbserver_writereg(char* commandbuffer);

// utils
static char* getmemorystring(unsigned int address, int len);
static char* gbdserver_readregs(char* commandbuffer);
static int gdbserver_calcchecksum(char *data);
static char* getregistersstring(int d0, int d1, int d2, int d3, int d4, int d5, int d6, int d7, int a0, int a1, int a2,
		int a3, int a4, int a5, int fp, int sp, int ps, int pc);

static void gbserver_set_breakpoint(uint32_t address);
static void gdbserver_clear_breakpoint(uint32_t address);
static void gdbserver_set_watchpoint(uint32_t address, unsigned int length, bool read, bool write);
static void gdbserver_clear_watchpoint(uint32_t address, unsigned int length, bool read, bool write);

// stuff that talks to gdb
static void gdbserver_readcommand(int s);
static bool gdbserver_readpacket(int s, char *buffer);
static bool gdbserver_sendpacket(int s, char* data);
static char* gdbserver_query(char* commandbuffer);

static void gdbserver_cleanup();
static void termination_handler(int signum);
static void registersighandler();

// commands
#define GDB_COMMAND_READREGISTERS	'g'
#define GDB_COMMAND_WRITEREGISTERS	'G'
#define GDB_COMMAND_READREGISTER	'p'
#define GDB_COMMAND_WRITEREGISTER	'P'
#define GDB_COMMAND_READMEMORY		'm'
#define GDB_COMMAND_WRITEMEMORY		'M'
#define GDB_COMMAND_CONTINUE		'c'
#define GDB_COMMAND_STEP			's'
#define GDB_COMMAND_HALTREASON		'?'
#define GDB_COMMAND_RESET			'r'
#define GDB_COMMAND_QUERY			'q'
#define GDB_COMMAND_DETACH			'D'
#define GDB_COMMAND_KILL			'k'
#define GDB_COMMAND_BREAKPOINTSET	'z'
#define GDB_COMMAND_BREAKPOINTUNSET	'Z'

#endif /* GDBSERVER_PRIVATE_H_ */
