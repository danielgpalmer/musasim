#include <sys/socket.h>
#include <sys/types.h>
#include <sys/time.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <stdint.h>
#include <string.h>
#include <unistd.h>
#include <inttypes.h>
#include <signal.h>
#include <fcntl.h>
#include <errno.h>
#include <poll.h>
#include <netinet/in.h>

#include <glib.h>

#include "sim.h"
#include "musashi/m68k.h"
#include "args.h"
#include "utils.h"
#include "hardware/cards/romcard.h"
#include "hardware/board.h"
#include "hardware/board_private.h"
#include "logging.h"
#include "gdbserver.h"
#include "gdbserver_private.h"
#include "profiler.h"
#include "throttler.h"
#include "ui/input.h"

#define GDB_RDP_BACKSTEP bs
#define GDB_RDP_BACKCONTINUE bc

static const char TAG[] = "gdbserver";

static bool tracing = false;

// some constants for the GDB proto
#define OK "OK"
static char GDBACK = '+';
static char GDBNAK = '-';
#define GDBPACKETSTART '$'
#define GDBDATAEND '#'
static char STOP_EXIT[] = "S02";
static char STOP_WEBROKE[] = "S05";

#define GDB_BREAKPOINTTYPE_SOFT 0
#define GDB_BREAKPOINTTYPE_WATCHPOINT_WRITE 2
#define GDB_BREAKPOINTTYPE_WATCHPOINT_READ 3
#define GDB_BREAKPOINTTYPE_WATCHPOINT_ACCESS 4

#define GDB_QUERY_MONITORCMD "qRcmd"

#define MAXPACKETLENGTH 256

// tcp/ip connection related stuff
static int socketlistening;
static int socketconnection;

// breakpoint stuff
static GSList* breakpoints;
static GSList* watchpoints_write;
static GSList* watchpoints_read;
static GSList* watchpoints_access;

static State state = INIT;

static bool interruptbreak = false;

int main(int argc, char* argv[]) {
	log_init();
	log_println(LEVEL_INFO, TAG,
			"musashi m68k emulator\tKarl Stenerud with patches from MAME up to 0105");
	log_println(LEVEL_INFO, TAG,
			"gdbserver for musashi\tDaniel Palmer (daniel@0x0f.com)");

	if (!args_parse(argc, argv)) {
		return 0;
	}

	if (state != EXIT)
		gdbserver_mainloop();

	gdbserver_cleanup();

	log_shutdown();
	return 0;
}

static void gdbserver_enablesigioforfd(int fd) {
	int flags = fcntl(socketconnection, F_GETFL, 0);
	fcntl(socketconnection, F_SETFL, flags | FASYNC);
}

static void gdbserver_disablesigioforfd(int fd) {
	int flags = fcntl(socketconnection, F_GETFL, 0);
	fcntl(socketconnection, F_SETFL, flags & ~FASYNC);
}

static void gdbserver_mainloop() {

	// the overall state of the program

	while (state != EXIT) {

		if (sim_has_quit()) {
			state = EXIT;
		}

		switch (state) {

		case INIT:
			sim_init();
			gdbserver_registersighandler();
			sim_reset();
			state = LISTENING;
			break;

		case LISTENING:
			log_println(LEVEL_INFO, TAG, "You may now connect GDB");
			if ((socketconnection = accept(socketlistening, NULL, NULL)) > 0) {
				log_println(LEVEL_INFO, TAG, "Got connection from GDB");

				fcntl(socketconnection, F_SETOWN, getpid());

				//if (setsockopt(socketconnection, SOL_SOCKET, SO_RCVTIMEO, &tout_val, sizeof(tout_val)) != 0) {
				//	log_println(LEVEL_WARNING, TAG, "Failed to set socket options");
				//}
				state = WAITING;
			} else {
				log_println(LEVEL_INFO, TAG, "accept() failed %d",
						socketconnection);
				state = EXIT;
			}
			break;

		case WAITING: {
			static char inputbuffer[MAXPACKETLENGTH];
			memset(inputbuffer, 0, MAXPACKETLENGTH);
			if (gdbserver_readpacket(socketconnection, inputbuffer)) {
				log_println(LEVEL_INFO, TAG, "processing packet from GDB");
				gdbserver_parsepacket(socketconnection, inputbuffer);
			}
			input_pump_and_checkquit();
		}
			break;

		case RUNNING:
			sim_tick();
			break;
		case STEPPING:
			sim_tick();
			break;

		case BREAKING:
			gdbserver_sendpacket(socketconnection, STOP_WEBROKE, false); // alert GDB to the fact that execution has stopped
			state = WAITING;
			break;

		default:
			break;
		}

	}

}

#define TOKENS(clear) (clear ? "z,#" : "Z,#")

static bool gdbserver_setorclearbreakpoint(bool clear, char* packet) {
	unsigned int type = strtoul(strtok(packet, TOKENS(clear)), NULL, 16);
	uint32_t breakaddress = strtoul(strtok(NULL, TOKENS(clear)), NULL, 16);
	unsigned int length = strtoul(strtok(NULL, TOKENS(clear)), NULL, 16); // length

	log_println(LEVEL_INFO, TAG,
			"GDB is %s a type %d breakpoint at 0x%08x with length %d",
			(clear ? "unsetting" : "setting"), type, breakaddress, length);

	switch (type) {
	case GDB_BREAKPOINTTYPE_SOFT:
		if (clear)
			gdbserver_clear_breakpoint(breakaddress);
		else
			gdbserver_set_breakpoint(breakaddress);
		break;
	case GDB_BREAKPOINTTYPE_WATCHPOINT_WRITE:
		if (clear)
			gdbserver_clear_watchpoint(breakaddress, length, false,
			true);
		else
			gdbserver_set_watchpoint(breakaddress, length, false, true);
		break;
	case GDB_BREAKPOINTTYPE_WATCHPOINT_READ:
		if (clear)
			gdbserver_clear_watchpoint(breakaddress, length, true,
			false);
		else
			gdbserver_set_watchpoint(breakaddress, length, true, false);
		break;
	case GDB_BREAKPOINTTYPE_WATCHPOINT_ACCESS:
		if (clear)
			gdbserver_clear_watchpoint(breakaddress, length, true,
			true);
		else
			gdbserver_set_watchpoint(breakaddress, length, true, true);
		break;
	default:
		log_println(LEVEL_INFO, TAG, "unsupported breakpoint type");
		return false;
	}

	return true;
}

static void gdbserver_parsepacket(int s, char* inputbuffer) {

	State newstate = WAITING;

	char* data = OK; // by default we send OK

	char command = inputbuffer[0];
	log_println(LEVEL_INFO, TAG, "command buffer %s", inputbuffer);
	switch (command) {
	case GDB_COMMAND_READREGISTERS:
		log_println(LEVEL_INFO, TAG, "GDB wants to read the registers");
		data = gbdserver_readregs(inputbuffer);
		break;
	case GDB_COMMAND_WRITEREGISTERS:
		log_println(LEVEL_INFO, TAG, "GDB wants to write the registers");
		break;
	case GDB_COMMAND_READREGISTER:
		log_println(LEVEL_INSANE, TAG, "GDB wants to read a single register");
		data = "00000000";
		break;
	case GDB_COMMAND_WRITEREGISTER:
		log_println(LEVEL_INSANE, TAG, "GDB wants to write a single register");
		gdbserver_writereg(inputbuffer);
		break;
	case GDB_COMMAND_READMEMORY:
		log_println(LEVEL_INSANE, TAG, "GDB wants to read from memory");
		data = gdbserver_readmem(inputbuffer);
		break;
	case GDB_COMMAND_WRITEMEMORY:
		log_println(LEVEL_INFO, TAG, "GDB wants to write to memory");
		gdbserver_parser_writemem(inputbuffer);
		break;
	case GDB_COMMAND_CONTINUE:
		log_println(LEVEL_INFO, TAG, "GDB wants execution to continue");
		gdbserver_enablesigioforfd(socketconnection);
		newstate = RUNNING;
		break;
	case GDB_COMMAND_STEP:
		log_println(LEVEL_INFO, TAG, "GDB wants execution to step");
		newstate = STEPPING;
		break;
	case GDB_COMMAND_HALTREASON:
		log_println(LEVEL_INFO, TAG, "GDB wants to know why we halted");
		data = STOP_WEBROKE;
		break;
	case GDB_COMMAND_RESET:
		log_println(LEVEL_INFO, TAG, "GDB wants the processor to reset");
		sim_reset();
		break;
	case GDB_COMMAND_QUERY:
		log_println(LEVEL_INFO, TAG, "GDB is querying something");
		data = gdbserver_handlequery(inputbuffer);
		break;
	case GDB_COMMAND_DETACH:
		log_println(LEVEL_INFO, TAG, "GDB is detaching!");
		newstate = LISTENING;
		break;
	case GDB_COMMAND_KILL:
		log_println(LEVEL_INFO, TAG, "GDB killed us");
		sim_reset();
		newstate = LISTENING;
		break;
	case GDB_COMMAND_BREAKPOINTSET:
		if (!gdbserver_setorclearbreakpoint(false, inputbuffer)) {
			data = "";
		}
		break;
	case GDB_COMMAND_BREAKPOINTUNSET:
		if (!gdbserver_setorclearbreakpoint(true, inputbuffer)) {
			data = "";
		}
		break;

		/*
		 case GDB_RDP_BACKCONTINUE:
		 break;

		 case GDB_RDP_BACKSTEP:
		 break;
		 */

	default:
		log_println(LEVEL_WARNING, TAG, "Command %c is unknown, packet was %s",
				command, inputbuffer);
		data = "";
		break;
	}

	if (gdbserver_sendpacket(s, data, true)) {
		state = newstate;
	} else {
		close(s);
		state = LISTENING;
	}

}

#define MAXSENDTRIES 10
static bool gdbserver_sendpacket(int s, char* data, bool ackincoming) {

	static char outputbuffer[MAXPACKETLENGTH];

	int triesleft = MAXSENDTRIES;
	int checksum = gdbserver_calcchecksum(data);
	bool ret = false;
	char res = GDBNAK;

	memset(outputbuffer, 0, MAXPACKETLENGTH);

	int outputlen = snprintf(outputbuffer, sizeof(outputbuffer), "$%s#%02x",
			data, checksum);

	if (ackincoming)
		write(s, &GDBACK, 1);

	while (res != GDBACK) {
		int result = write(s, outputbuffer, outputlen);
		if (result != outputlen)
			log_println(LEVEL_WARNING, TAG,
					"expected to get %d but got %d from write, errno %d",
					outputlen, result, errno);
		else {

			result = read(s, &res, 1);

			log_println(LEVEL_WARNING, TAG, "%d result %d errno %c", result,
			errno, res);

			if (result == 0) {
				log_println(LEVEL_WARNING, TAG, "EOF when reading from gdb");
				goto exit;
			} else if (result < 0) {
				log_println(LEVEL_WARNING, TAG,
						"Error reading from socket! result %d, errno %d",
						result, errno);
				goto exit;
			}
		}
		triesleft--;
		if (triesleft == 0) {
			log_println(LEVEL_WARNING, TAG,
					"Sent packet [%s] %d times, giving up!", outputbuffer,
					MAXSENDTRIES);
			goto exit;
		}
	}
	ret = true;
	exit: return ret;
}

static bool gdbserver_readpacket(int s, char *buffer) {

// FIXME I have no idea WTF I was thinking when I wrote this.. clean this shit up

// the state of the packet reader
	typedef enum ReadState {
		WAITINGFORSTART, READINGPACKET, CHECKSUMDIGITONE, CHECKSUMDIGITTWO, DONE
	} ReadState;

	ReadState readstate = WAITINGFORSTART;
	static char readbuffer[MAXPACKETLENGTH];
//static char checksum[3];
	int bytessofar = 0;
	int bufferpos = 0;
//int check = 0;
	bool ret = false;

	while (readstate != DONE) {

		int res = read(s, readbuffer + bytessofar, MAXPACKETLENGTH);

		if (res < 0) {
			//write(socketconnection, &GDBNAK, 1);
			log_println(LEVEL_INFO, TAG,
					"failed to read from socket result %d errno %d", res,
					errno);
			break;
		}

		bytessofar += res;
		switch (readstate) {
		case WAITINGFORSTART:
			// Wait until we get a dollar
			for (; bufferpos < bytessofar; bufferpos++) {
				if (readbuffer[bufferpos] == GDBPACKETSTART) {
					readstate = READINGPACKET;
					break;
				}
			}
			// If we are still waiting or we ate all the buffer here.. read some more from the socket
			if (readstate == WAITINGFORSTART || bufferpos == bytessofar) {
				break;
			} else {
				bufferpos++;
			}
			/* no break */

		case READINGPACKET:
			for (; bufferpos < bytessofar; bufferpos++) {
				if (readbuffer[bufferpos] == GDBDATAEND) {
					readstate = CHECKSUMDIGITONE;
					break;
				} else {
					*buffer++ = readbuffer[bufferpos];
				}
			}

			if (readstate == READINGPACKET || bufferpos > bytessofar) {
				break;
			}

			else {
				*buffer = '\0';
				bufferpos++;
			}
			/* no break */

		case CHECKSUMDIGITONE:

			if (bufferpos > bytessofar) {
				break;
			}

			//checksum[0] = readbuffer[bufferpos];
			//printf("checkone %c\n", readbuffer[bufferpos]);
			bufferpos++;

			readstate = CHECKSUMDIGITTWO;
			/* no break */

		case CHECKSUMDIGITTWO:

			if (bufferpos > bytessofar) {
				break;
			}
			//checksum[1] = readbuffer[bufferpos];
			//printf("checktwo %c\n", readbuffer[bufferpos]);

			readstate = DONE;
			/* no break */
		case DONE:

			//checksum[2] = '\0';
			//check = sscanf(checksum, "%02x", &check);
			//log_println(LEVEL_DEBUG, TAG, "check sum is 0x%02x", check);

			ret = true;
		}
	}

	return ret;
}

static int gdbserver_calcchecksum(char *data) {

	int len = strlen(data);

	int checksum = 0;
	int i;
	for (i = 0; i < len; i++) {
		checksum += data[i];

	}
	checksum = checksum & 0xFF;

	return checksum;
}

static void gdbserver_cleanup() {
//shutdown(socketconnection, SHUT_RDWR);
//close(socketlistening);
//close(socketconnection);

//char xxx[1024];
//GSList* iterator;
//for (iterator = trace; iterator; iterator = iterator->next) {
//	uint32_t pc = GPOINTER_TO_UINT(iterator->data);
//	m68k_disassemble(xxx, pc, M68K_CPU_TYPE_68000);
//	log_println(LEVEL_INFO, TAG, "0x%08x; %s", pc, xxx);
//}

	log_println(LEVEL_INFO, TAG, "Cleaning up");

	shutdown(socketconnection, SHUT_RDWR);
	shutdown(socketlistening, SHUT_RDWR);
	state = EXIT;

	profiler_flush();

	g_slist_free(breakpoints);
	g_slist_free(watchpoints_access);
	g_slist_free(watchpoints_read);
	g_slist_free(watchpoints_write);

}

static void gdbserver_termination_handler(int signum) {
	log_println(LEVEL_INFO, TAG, "Caught interrupt");
	if (state == RUNNING) {
		gdbserver_sendpacket(socketconnection, STOP_EXIT, false);
	}
	sim_quit();
}

static void gdbserver_io_handler(int signum) {
	log_println(LEVEL_INFO, TAG, "IO has happened");
	if (state == RUNNING) {
		log_println(LEVEL_INFO, TAG, "IO while running, breaking");
		state = BREAKING;
		gdbserver_disablesigioforfd(socketconnection);
	}
}

static void gdbserver_registersighandler() {

	struct sigaction new_action, old_action, io_action;

	/* Set up the structure to specify the new action. */
	new_action.sa_handler = gdbserver_termination_handler;
	sigemptyset(&new_action.sa_mask);
	new_action.sa_flags = 0;

	io_action.sa_handler = gdbserver_io_handler;
	sigemptyset(&io_action.sa_mask);
	io_action.sa_flags = 0;

	sigaction(SIGINT, NULL, &old_action);
	if (old_action.sa_handler != SIG_IGN)
		sigaction(SIGINT, &new_action, NULL);

	sigaction(SIGHUP, NULL, &old_action);
	if (old_action.sa_handler != SIG_IGN)
		sigaction(SIGHUP, &new_action, NULL);

	sigaction(SIGTERM, NULL, &old_action);
	if (old_action.sa_handler != SIG_IGN)
		sigaction(SIGTERM, &new_action, NULL);

	sigaction(SIGIO, NULL, &old_action);
	if (old_action.sa_handler != SIG_IGN)
		sigaction(SIGIO, &io_action, NULL);

}

#define REGSTRINGLEN 168

static char* gdbserver_getregistersstring(int d0, int d1, int d2, int d3,
		int d4, int d5, int d6, int d7, int a0, int a1, int a2, int a3, int a4,
		int a5, int fp, int sp, int ps, int pc) {

	static char registersstring[REGSTRINGLEN];
	memset(registersstring, 0, REGSTRINGLEN);

	snprintf(registersstring, REGSTRINGLEN,
			"%08x%08x%08x%08x%08x%08x%08x%08x%08x%08x%08x%08x%08x%08x%08x%08x%08x%08x",
			d0, d1, d2, d3, d4, d5, d6, d7, a0, a1, a2, a3, a4, a5, fp, sp, ps,
			pc);

	return registersstring;

}

static char* getmemorystring(unsigned int address, int len) {

	static char memorystring[256];
	memset(memorystring, 0, 256);

	unsigned int byte;
	int i;
	for (i = 0; i < len; i++) {
		byte = (board_read_byte_internal(address + i, true, NULL));
		sprintf(&memorystring[i * 2], "%02x", byte);
	}

	return memorystring;

}

static inline void gdbserver_set_breakpoint(uint32_t address) {
	breakpoints = g_slist_append(breakpoints, GUINT_TO_POINTER(address));
}

static inline void gdbserver_clear_breakpoint(uint32_t address) {
	breakpoints = g_slist_remove(breakpoints, GUINT_TO_POINTER(address));
}

static inline GSList* gdbserver_addwatchpoint(GSList* list, uint32_t address,
		unsigned int length) {
	watchpoint* wp = malloc(sizeof(wp));
	wp->address = address;
	wp->length = length;
	return g_slist_append(list, wp);
}

static inline GSList* gdbserver_clearwatchpoint(GSList* list, uint32_t address,
		unsigned int length) {
	GSList* iterator;
	for (iterator = list; iterator; iterator = iterator->next) {
		watchpoint* wp = iterator->data;
		if (wp->address == address && wp->length == length) {
			free(wp);
			return g_slist_remove(list, wp);
		}
	}
	return list;
}

static bool gdbserver_checkwatchpoints(GSList* list, uint32_t address, int size) {
	GSList* iterator;
	for (iterator = list; iterator; iterator = iterator->next) {
		watchpoint* wp = iterator->data;

		int wpmin = wp->address;
		int wpmax = wp->address + (wp->length - 1);
		int actionmin = address;
		int actionmax = address + (size - 1);

		printf("Checking watchpoint 0x%08x:%d with 0x%08x:%d\n", wp->address,
				wp->length, address, size);

		printf("watch point range %x -> %x\n", wpmin, wpmax);
		printf("action range %x -> %x\n", actionmin, actionmax);

		if ((actionmin >= wpmin && actionmin <= wpmax)
				|| (actionmax >= wpmin && actionmax <= wpmax)) {

			printf("ranges overlap\n");

			return true;
		}
	}
	return false;
}

static void gdbserver_set_watchpoint(uint32_t address, unsigned int length,
bool read, bool write) {
	if (read && write)
		watchpoints_access = gdbserver_addwatchpoint(watchpoints_access,
				address, length);
	else if (read)
		watchpoints_read = gdbserver_addwatchpoint(watchpoints_read, address,
				length);
	else if (write)
		watchpoints_write = gdbserver_addwatchpoint(watchpoints_write, address,
				length);
}

static void gdbserver_clear_watchpoint(uint32_t address, unsigned int length,
bool read, bool write) {
	if (read && write)
		watchpoints_access = gdbserver_clearwatchpoint(watchpoints_access,
				address, length);
	else if (read)
		watchpoints_read = gdbserver_clearwatchpoint(watchpoints_read, address,
				length);
	else if (write)
		watchpoints_write = gdbserver_clearwatchpoint(watchpoints_write,
				address, length);
}

static void gdbserver_check_watchpoints(uint32_t address, uint32_t value,
bool write, int size) {
	static char stopreply[256];

	if (gdbserver_checkwatchpoints(watchpoints_access, address, size)) {
		m68k_end_timeslice();
		log_println(LEVEL_INFO, TAG, "Access at 0x%08x;", address);
		state = WAITING;
		sprintf(stopreply, "T05awatch:%08x;", address);
		gdbserver_sendpacket(socketconnection, stopreply, false);
	}

	if (write) {
		if (gdbserver_checkwatchpoints(watchpoints_write, address, size)) {
			m68k_end_timeslice();
			log_println(LEVEL_INFO, TAG, "Write 0x%08x to 0x%08x PC[0x%08x]",
					value, address, m68k_get_reg(NULL, M68K_REG_PC));
			state = WAITING;
			sprintf(stopreply, "T05watch:%08x;", address);
			gdbserver_sendpacket(socketconnection, stopreply, false);
		}
	}

	else {
		if (gdbserver_checkwatchpoints(watchpoints_read, address, size)) {
			m68k_end_timeslice();
			log_println(LEVEL_INFO, TAG, "Read at 0x%08x;", address);
			state = WAITING;
			sprintf(stopreply, "T05rwatch:%08x;", address);
			gdbserver_sendpacket(socketconnection, stopreply, false);
		}
	}
}

static char* gbdserver_munchhexstring(char* buffer, int* len) {
	static char string[256];
	int stringpos = 0;

	memset(string, 0, 256);
	unsigned int current;
	while (sscanf(buffer, "%2x", &current) > 0) {
		string[stringpos] = (char) current;
		stringpos += 1;
		buffer += 2;
	}

	*len = stringpos;
	return string;
}

#define RESET "reset"
#define LOAD "load"
#define STFU "stfu"
#define TALKTOME "talktome"
#define INTERRUPTBREAK "interruptbreak"
#define THROTTLE "throttle"
#define TRACE "trace"
#define ON "on"
#define OFF "off"

static char* gdbserver_handlemonitorcommand(char* commandbuffer) {

	char* ret = "";

	char* offset = strchr(commandbuffer, ',') + 1;

	int monitorcommandlen = 0;
	char* monitorcommand = gbdserver_munchhexstring(offset, &monitorcommandlen);
	log_println(LEVEL_INFO, TAG, "GDB is sending a monitor command; %s (%s)",
			offset, monitorcommand);

	if (strncmp(monitorcommand, LOAD, sizeof(LOAD)) == 0) {
		log_println(LEVEL_INFO, TAG,
				"User has requested that a new binary is loaded into ROM");
		romcard_loadrom(monitorcommand + 5, false);
		ret = OK;
	}

	else if (strncmp(monitorcommand, RESET, sizeof(RESET)) == 0) {
		log_println(LEVEL_INFO, TAG, "User has requested the CPU is reset");
		sim_reset();
		ret = OK;
	}

	else if (strncmp(monitorcommand, STFU, sizeof(STFU)) == 0) {
		log_println(LEVEL_INFO, TAG, "User has requested we keep quiet");
		ret = OK;
	}

	else if (strncmp(monitorcommand, TALKTOME, sizeof(TALKTOME)) == 0) {
		log_println(LEVEL_INFO, TAG, "User wants to know what's going down");
		ret = OK;
	}

	else if (strncmp(monitorcommand, INTERRUPTBREAK, sizeof(INTERRUPTBREAK) - 1)
			== 0) {
		char* mode = monitorcommand + sizeof(INTERRUPTBREAK);
		if (strncmp(mode, ON, sizeof(ON)) == 0) {
			log_println(LEVEL_INFO, TAG, "User wants break on interrupts");
			interruptbreak = true;
			ret = OK;
		} else if (strncmp(mode, OFF, sizeof(OFF)) == 0) {
			log_println(LEVEL_INFO, TAG,
					"User doesn't want to break on interrupts");
			interruptbreak = false;
			ret = OK;
		} else
			log_println(LEVEL_INFO, TAG, "bad interruptbreak mode \"%s\"",
					mode);
	}

	else if (strncmp(monitorcommand, THROTTLE, sizeof(THROTTLE) - 1) == 0) {
		char* mode = monitorcommand + sizeof(THROTTLE);
		if (strncmp(mode, ON, sizeof(ON)) == 0) {
			throttler_enable(true);
			ret = OK;
		} else if (strncmp(mode, OFF, sizeof(OFF)) == 0) {
			throttler_enable(false);
			ret = OK;
		} else
			log_println(LEVEL_INFO, TAG, "bad throttle mode \"%s\"", mode);
	}

	else if (strncmp(monitorcommand, TRACE, sizeof(TRACE) - 1) == 0) {
		char* mode = monitorcommand + sizeof(TRACE);
		if (strncmp(mode, ON, sizeof(ON)) == 0) {
			tracing = true;
			ret = OK;
		} else if (strncmp(mode, OFF, sizeof(OFF)) == 0) {
			tracing = false;
			ret = OK;
		} else
			log_println(LEVEL_INFO, TAG, "bad trace mode \"%s\"", mode);
	} else
		log_println(LEVEL_INFO, TAG, "Unknown monitor command \"%s\"",
				commandbuffer);

	return ret;
}

static char* gdbserver_handlequery(char* commandbuffer) {
	if (strncmp(commandbuffer, GDB_QUERY_MONITORCMD,
			strlen(GDB_QUERY_MONITORCMD)) == 0) {
		return gdbserver_handlemonitorcommand(commandbuffer);
	} else
		return "";
}

static char* gbdserver_readregs(char* commandbuffer) {
	return gdbserver_getregistersstring(m68k_get_reg(NULL, M68K_REG_D0),
			m68k_get_reg(NULL, M68K_REG_D1), m68k_get_reg(NULL, M68K_REG_D2),
			m68k_get_reg(NULL, M68K_REG_D3), m68k_get_reg(NULL, M68K_REG_D4),
			m68k_get_reg(NULL, M68K_REG_D5), m68k_get_reg(NULL, M68K_REG_D6),
			m68k_get_reg(NULL, M68K_REG_D7), m68k_get_reg(NULL, M68K_REG_A0),
			m68k_get_reg(NULL, M68K_REG_A1), m68k_get_reg(NULL, M68K_REG_A2),
			m68k_get_reg(NULL, M68K_REG_A3), m68k_get_reg(NULL, M68K_REG_A4),
			m68k_get_reg(NULL, M68K_REG_A5), m68k_get_reg(NULL, M68K_REG_A6), // fp
			m68k_get_reg(NULL, M68K_REG_A7), // sp
			m68k_get_reg(NULL, M68K_REG_SR), // ps
			m68k_get_reg(NULL, M68K_REG_PC));
}

static void gdbserver_writereg(char* commandbuffer) {

	uint8_t reg;
	uint32_t value;
	sscanf(&commandbuffer[1], "%"SCNx8"=%"SCNx32, &reg, &value);
	log_println(LEVEL_INFO, TAG, "reg %"PRIx8" value 0x%"PRIx32, reg, value);

// todo make this smaller..
	switch (reg) {
	case 0:
		m68k_set_reg(M68K_REG_D0, value);
		break;
	case 1:
		m68k_set_reg(M68K_REG_D1, value);
		break;
	case 2:
		m68k_set_reg(M68K_REG_D2, value);
		break;
	case 3:
		m68k_set_reg(M68K_REG_D3, value);
		break;
	case 4:
		m68k_set_reg(M68K_REG_D4, value);
		break;
	case 5:
		m68k_set_reg(M68K_REG_D5, value);
		break;
	case 6:
		m68k_set_reg(M68K_REG_D6, value);
		break;
	case 7:
		m68k_set_reg(M68K_REG_D7, value);
		break;
	case 8:
		m68k_set_reg(M68K_REG_A0, value);
		break;
	case 9:
		m68k_set_reg(M68K_REG_A1, value);
		break;
	case 0xa:
		m68k_set_reg(M68K_REG_A2, value);
		break;
	case 0xb:
		m68k_set_reg(M68K_REG_A3, value);
		break;
	case 0xc:
		m68k_set_reg(M68K_REG_A4, value);
		break;
	case 0xd:
		m68k_set_reg(M68K_REG_A5, value);
		break;
	case 0xe: // fp
		m68k_set_reg(M68K_REG_A6, value);
		break;
	case 0xf:
		m68k_set_reg(M68K_REG_A7, value);
		break;
	case 0x11:
		m68k_set_reg(M68K_REG_PC, value);
		break;
	default:
		log_println(LEVEL_INFO, TAG, "what is %"PRIx8, reg);
		break;
	}

}

static char* gdbserver_parser_writemem(char* commandbuffer) {
	return NULL;
}

static char* gdbserver_readmem(char* commandbuffer) {
	char *address = strtok(&commandbuffer[1], "m,#");
	char *size = strtok(NULL, "m,#");
	unsigned int ad = strtoul(address, NULL, 16);
	int sz = strtol(size, NULL, 16);
	return getmemorystring(ad, sz);
}

static void gdbserver_check_breakpoints(uint32_t pc) {

	char disasmbuffer[256];

	if (state == STEPPING) {
		m68k_end_timeslice();
		m68k_disassemble(disasmbuffer, pc, M68K_CPU_TYPE_68000);
		log_println(LEVEL_INFO, TAG, "Stepped to 0x%08x; %s", pc, disasmbuffer);
		utils_printregisters();
		state = BREAKING;
	}

	GSList* iterator;
	for (iterator = breakpoints; iterator; iterator = iterator->next) {
		if (GPOINTER_TO_UINT(iterator->data) == pc) {
			m68k_end_timeslice();
			m68k_disassemble(disasmbuffer, pc, M68K_CPU_TYPE_68000);
			log_println(LEVEL_INFO, TAG, "Broke at 0x%08x; %s", pc,
					disasmbuffer);
			utils_printregisters();
			state = WAITING;
			gdbserver_sendpacket(socketconnection, STOP_WEBROKE, false);
			break;
		}
	}
}

// externally visible stuff
void gdbserver_enteringinterrupt(int irq) {
	if (interruptbreak) {
		log_println(LEVEL_INFO, TAG, "irq %d", irq);
		gdb_break("Entering interrupt");
	}
}

void gdbserver_instruction_hook_callback() {
	uint32_t pc = m68k_get_reg(NULL, M68K_REG_PC);
	gdbserver_check_breakpoints(pc);
	profiler_onpcchange(pc);
}

void gdbserver_setport(int port) {
	struct sockaddr_in servaddr;
	if ((socketlistening = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
		log_println(LEVEL_WARNING, TAG, "Failed to create socket");
		state = EXIT;
	} else {
		memset(&servaddr, 0, sizeof(servaddr));
		servaddr.sin_family = AF_INET;
		servaddr.sin_addr.s_addr = htonl(INADDR_LOOPBACK);
		servaddr.sin_port = htons(port);

		if (bind(socketlistening, (struct sockaddr *) &servaddr,
				sizeof(servaddr)) < 0) {
			log_println(LEVEL_WARNING, TAG, "bind() failed");
			state = EXIT;
			return;
		}

		if (listen(socketlistening, 0) < 0) {
			log_println(LEVEL_WARNING, TAG, "listen() failed");
			state = EXIT;
			return;
		}

		log_println(LEVEL_INFO, TAG, "Listening for GDB connection on %d",
				port);
	}
}

void gdb_hitstop() {
	log_println(LEVEL_INFO, TAG, "hit stop");
	cpu_pulse_stop();
}

void gdb_onpcmodified(uint32_t a) {
}

void gdb_break(const char* reason) {
	log_println(LEVEL_INFO, TAG, "gdb_break(%s)", reason);
	m68k_end_timeslice();
	utils_printregisters();
	state = BREAKING;
}

uint8_t gdbserver_m68k_read_byte(uint32_t address) {
	gdbserver_check_watchpoints(address, 0, false, 1);
	return board_read_byte_cpu(address);
}

uint16_t gdbserver_m68k_read_word(uint32_t address) {
	gdbserver_check_watchpoints(address, 0, false, 2);
	return board_read_word_cpu(address);
}

uint32_t gdbserver_m68k_read_long(uint32_t address) {
	gdbserver_check_watchpoints(address, 0, false, 4);
	return board_read_long_cpu(address);
}

void gdbserver_m68k_write_byte(uint32_t address, uint8_t value) {
	gdbserver_check_watchpoints(address, value, true, 1);
	return board_write_byte_cpu(address, value);
}

void gdbserver_m68k_write_word(uint32_t address, uint16_t value) {
	gdbserver_check_watchpoints(address, value, true, 2);
	return board_write_word_cpu(address, value);
}

void gdbserver_m68k_write_long(uint32_t address, uint32_t value) {
	gdbserver_check_watchpoints(address, value, true, 4);
	return board_write_long_cpu(address, value);
}
