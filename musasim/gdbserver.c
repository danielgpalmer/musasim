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
#include <signal.h>
#include <fcntl.h>
#include <errno.h>
#include <netinet/in.h>

#include <glib.h>

#include "sim.h"
#include "musashi/m68k.h"
#include "args.h"

#include "hardware/cards/romcard.h"
#include "logging.h"

#include "gdbserver.h"
#include "profiler.h"

#define GDB_RDP_BACKSTEP bs
#define GDB_RDP_BACKCONTINUE bc

// some constants for the GDB proto
static char OK[] = "OK";
static char GDBACK = '+';
static char GDBNAK = '-';
#define GDBPACKETSTART '$'
#define GDBDATAEND '#'
static char STOP_WEBROKE[] = "S05";

#define GDB_BREAKPOINTTYPE_SOFT 0
#define GDB_BREAKPOINTTYPE_WATCHPOINT_WRITE 2
#define GDB_BREAKPOINTTYPE_WATCHPOINT_READ 3
#define GDB_BREAKPOINTTYPE_WATCHPOINT_ACCESS 4

#define MAXPACKETLENGTH 256

static void gdbserver_cleanup();
static void termination_handler(int signum);
static void registersighandler();

// tcp/ip connection related stuff
static int socketlistening;
static int socketconnection;

// stuff that talks to gdb
static void gdbserver_readcommand(int s);
static bool gdbserver_readpacket(int s, char *buffer);
static bool gdbserver_sendpacket(int s, char* data);
static char* gdbserver_query(char* commandbuffer);

// breakpoint stuff
static GSList* breakpoints;
static GSList* watchpoints_write;
static GSList* watchpoints_read;
static GSList* watchpoints_access;

typedef struct {
	uint32_t address;
	unsigned int length;
} watchpoint;

static void gbserver_set_breakpoint(uint32_t address);
static void gdbserver_clear_breakpoint(uint32_t address);
static void gdbserver_set_watchpoint(uint32_t address, unsigned int length, bool read, bool write);
static void gdbserver_clear_watchpoint(uint32_t address, unsigned int length, bool read, bool write);

// stuff that pokes the sim
static char* readmem(char* commandbuffer);

// utils
static char* getmemorystring(unsigned int address, int len);
static char* gbdserver_readregs(char* commandbuffer);
static int gdbserver_calcchecksum(char *data);
static char* getregistersstring(int d0, int d1, int d2, int d3, int d4, int d5, int d6, int d7, int a0, int a1, int a2,
		int a3, int a4, int a5, int fp, int sp, int ps, int pc);

typedef enum State {
	INIT, LISTENING, WAITING, RUNNING, STEPPING, BREAKING, EXIT

} State;
static State state = INIT;
static const char TAG[] = "gdbserver";

static void mainloop() {

	// the overall state of the program

	//struct timeval tout_val;
	//tout_val.tv_sec = 2;
	//tout_val.tv_usec = 0;

	while (state != EXIT) {

		if (sim_has_quit()) {
			state = EXIT;
		}

		switch (state) {

			case INIT:
				sim_init();
				registersighandler();
				sim_reset();
				state = LISTENING;
				break;

			case LISTENING:
				if ((socketconnection = accept(socketlistening, NULL, NULL)) > 0) {
					log_println(LEVEL_INFO, TAG, "Got connection from GDB");

					fcntl(socketconnection, F_SETOWN, getpid());
					int flags = fcntl(socketconnection, F_GETFL, 0);
					fcntl(socketconnection, F_SETFL, flags | FASYNC);

					//if (setsockopt(socketconnection, SOL_SOCKET, SO_RCVTIMEO, &tout_val, sizeof(tout_val)) != 0) {
					//	log_println(LEVEL_WARNING, TAG, "Failed to set socket options");
					//}
					state = WAITING;
				}
				else {
					printf("YAR!\n");
					state = EXIT;
				}
				break;

			case WAITING:
				gdbserver_readcommand(socketconnection);
				break;

			case RUNNING:
				sim_tick();
				break;
			case STEPPING:
				sim_tick();
				break;

			case BREAKING:
				gdbserver_sendpacket(socketconnection, STOP_WEBROKE); // alert GDB to the fact that execution has stopped
				state = WAITING;
				break;

			default:
				break;
		}

	}

}

int main(int argc, char* argv[]) {

	log_println(LEVEL_INFO, TAG, "musashi m68k emulator\tKarl Stenerud with patches from MAME up to 0105");
	log_println(LEVEL_INFO, TAG, "gdbserver for musashi\tDaniel Palmer (daniel@0x0f.com)");

	if (!args_parse(argc, argv)) {
		return 0;
	}

	mainloop();

	gdbserver_cleanup();
	return 0;

}

static bool gdbserver_setbreakpoint(char* packet) {
	const char tokens[] = "Z,#";
	unsigned int type = strtoul(strtok(packet, tokens), NULL, 16);
	uint32_t breakaddress = strtoul(strtok(NULL, tokens), NULL, 16);
	unsigned int length = strtoul(strtok(NULL, tokens), NULL, 16); // length

	printf("GDB is setting a type %d breakpoint at 0x%08x with length %d\n", type, breakaddress, length);

	switch (type) {
		case GDB_BREAKPOINTTYPE_SOFT:
			gbserver_set_breakpoint(breakaddress);
			break;

		case GDB_BREAKPOINTTYPE_WATCHPOINT_WRITE:
			gdbserver_set_watchpoint(breakaddress, length, false, true);
			break;
		case GDB_BREAKPOINTTYPE_WATCHPOINT_READ:
			gdbserver_set_watchpoint(breakaddress, length, true, false);
			break;
		case GDB_BREAKPOINTTYPE_WATCHPOINT_ACCESS:
			gdbserver_set_watchpoint(breakaddress, length, true, true);
			break;
		default:
			printf("unsupported breakpoint type\n");
			return false;
	}

	return true;
}

static bool gdbserver_unsetbreakpoint(char* packet) {
	const char tokens[] = "z,#";
	unsigned int type = strtoul(strtok(packet, tokens), NULL, 16);
	uint32_t breakaddress = strtoul(strtok(NULL, tokens), NULL, 16);
	unsigned int length = strtoul(strtok(NULL, tokens), NULL, 16); // length

	printf("GDB is unsetting a type %d breakpoint at 0x%08x with length %d\n", type, breakaddress, length);

	switch (type) {
		case GDB_BREAKPOINTTYPE_SOFT:
			gdbserver_clear_breakpoint(breakaddress);
			break;
		case GDB_BREAKPOINTTYPE_WATCHPOINT_WRITE:
			gdbserver_clear_watchpoint(breakaddress, length, false, true);
			break;
		case GDB_BREAKPOINTTYPE_WATCHPOINT_READ:
			gdbserver_clear_watchpoint(breakaddress, length, true, false);
			break;
		case GDB_BREAKPOINTTYPE_WATCHPOINT_ACCESS:
			gdbserver_clear_watchpoint(breakaddress, length, true, true);
			break;
		default:
			printf("unsupported breakpoint type\n");
			return false;

	}

	return true;

}

static void gdbserver_readcommand(int s) {

	static char inputbuffer[MAXPACKETLENGTH];
	memset(inputbuffer, 0, MAXPACKETLENGTH);

	State newstate = WAITING;

	if (gdbserver_readpacket(s, inputbuffer)) {

		char* data = OK; // by default we send OK

		char command = inputbuffer[0];
		switch (command) {
			case 'g':
				printf("GDB wants to read the registers\n");
				data = gbdserver_readregs(inputbuffer);
				break;
			case 'G':
				printf("GDB wants to write the registers\n");
				break;
			case 'p':
				log_println(LEVEL_INSANE, TAG, "GDB wants to read a single register");
				data = "00000000";
				break;
			case 'P':
				log_println(LEVEL_INSANE, TAG, "GDB wants to write a single register");
				break;
			case 'm':
				log_println(LEVEL_INSANE, TAG, "GDB wants to read from memory");
				data = readmem(inputbuffer);
				break;
			case 'M':
				printf("GDB wants to write to memory\n");

				break;
			case 'c':
				log_println(LEVEL_INFO, TAG, "GDB wants execution to continue");
				newstate = RUNNING;
				break;
			case 's':
				printf("GDB wants execution to step\n");
				newstate = STEPPING;
				break;
			case '?':
				printf("GDB wants to know why we halted\n");
				data = STOP_WEBROKE;
				break;
			case 'r':
				printf("GDB wants the processor to reset\n");

				sim_reset();
				break;

			case 'q':
				printf("GDB is querying something\n");
				data = gdbserver_query(inputbuffer);
				break;

			case 'D':
				printf("GDB is detaching!\n");
				newstate = LISTENING;
				break;

			case 'k':
				printf("GDB killed us\n");
				newstate = LISTENING;
				break;

			case 'z':
				if (!gdbserver_unsetbreakpoint(inputbuffer)) {
					data = "";
				}
				break;

			case 'Z':
				if (!gdbserver_setbreakpoint(inputbuffer)) {
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

				fprintf(stderr, "Command %c is unknown, packet was %s\n", command, inputbuffer);
				data = "";
				break;
		}

		if (gdbserver_sendpacket(s, data)) {
			state = newstate;
		}
		else {
			close(s);
			state = LISTENING;
		}
	}
	else {
		log_println(LEVEL_INFO, TAG, "no packet");
	}

}

#define MAXSENDTRIES 10
static bool gdbserver_sendpacket(int s, char* data) {

	static char outputbuffer[MAXPACKETLENGTH];
	memset(outputbuffer, 0, MAXPACKETLENGTH);
	int triesleft = MAXSENDTRIES;
	int outputlen = sprintf(outputbuffer, "$%s#", data);
	outputlen += sprintf(outputbuffer + outputlen, "%02x", gdbserver_calcchecksum(data));

	char res = GDBNAK;
	while (res != GDBACK) {
		write(s, outputbuffer, outputlen);
		int result = read(s, &res, 1);
		if (result == 0) {
			printf("EOF!\n");
			return false;
		}
		else if (result < 0) {
			printf("Error reading from socket!\n");
			return false;
		}

		triesleft--;
		if (triesleft == 0) {
			log_println(LEVEL_WARNING, TAG, "Sent packet [%s] %d times, giving up!", &outputbuffer, MAXSENDTRIES);
			return false;
		}
	}
	return true;
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

	while (readstate != DONE) {

		int res = read(s, readbuffer + bytessofar, MAXPACKETLENGTH);

		if (res < 0) {
			log_println(LEVEL_INFO, TAG, "failed to read from socket %d", res);
			write(socketconnection, &GDBNAK, 1);
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
				}
				else {
					bufferpos++;
				}

			case READINGPACKET:
				for (; bufferpos < bytessofar; bufferpos++) {
					if (readbuffer[bufferpos] == GDBDATAEND) {
						readstate = CHECKSUMDIGITONE;
						break;
					}
					else {
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

			case CHECKSUMDIGITONE:

				if (bufferpos > bytessofar) {
					break;
				}

				//checksum[0] = readbuffer[bufferpos];
				//printf("checkone %c\n", readbuffer[bufferpos]);
				bufferpos++;

				readstate = CHECKSUMDIGITTWO;

			case CHECKSUMDIGITTWO:

				if (bufferpos > bytessofar) {
					break;
				}
				//checksum[1] = readbuffer[bufferpos];
				//printf("checktwo %c\n", readbuffer[bufferpos]);

				readstate = DONE;
			case DONE:

				//checksum[2] = '\0';
				//check = sscanf(checksum, "%02x", &check);
				//log_println(LEVEL_DEBUG, TAG, "check sum is 0x%02x", check);
				write(s, &GDBACK, 1);
				return true;
		}
	}

	return false;
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
	sim_quit();

	profiler_flush();

	g_slist_free(breakpoints);
	g_slist_free(watchpoints_access);
	g_slist_free(watchpoints_read);
	g_slist_free(watchpoints_write);

}

void termination_handler(int signum) {

	printf("Caught interrupt\n");
	shutdown(socketconnection, SHUT_RDWR);
	shutdown(socketlistening, SHUT_RDWR);
	state = EXIT;
}

void io_handler(int signum) {
//printf("IO\n");
	if (state == RUNNING) {
		printf("IO has happened on the the socket, breaking\n");
		state = BREAKING;
	}
}

void registersighandler() {

// stolen from; http://www.gnu.org/s/hello/manual/libc/Sigaction-Function-Example.html

	struct sigaction new_action, old_action, io_action;

	/* Set up the structure to specify the new action. */
	new_action.sa_handler = termination_handler;
	sigemptyset(&new_action.sa_mask);
	new_action.sa_flags = 0;

	io_action.sa_handler = io_handler;
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

static char* getregistersstring(int d0, int d1, int d2, int d3, int d4, int d5, int d6, int d7, int a0, int a1, int a2,
		int a3, int a4, int a5, int fp, int sp, int ps, int pc) {

	static char registersstring[168];
	memset(registersstring, 0, 168);

	sprintf(registersstring, "%08x%08x%08x%08x%08x%08x%08x%08x%08x%08x%08x%08x%08x%08x%08x%08x%08x%08x", d0, d1, d2, d3,
			d4, d5, d6, d7, a0, a1, a2, a3, a4, a5, fp, sp, ps, pc);

	return registersstring;

}

static char* getmemorystring(unsigned int address, int len) {

	static char memorystring[256];
	memset(memorystring, 0, 256);

	unsigned int byte;
	int i;
	for (i = 0; i < len; i++) {
		byte = (board_read_byte(address + i));
		sprintf(&memorystring[i * 2], "%02x", byte);
	}

	return memorystring;

}

void gbserver_set_breakpoint(uint32_t address) {
	breakpoints = g_slist_append(breakpoints, GUINT_TO_POINTER(address));
}

void gdbserver_clear_breakpoint(uint32_t address) {
	breakpoints = g_slist_remove(breakpoints, GUINT_TO_POINTER(address));
}

static GSList* addwatchpoint(GSList* list, uint32_t address, unsigned int length) {

	watchpoint* wp = malloc(sizeof(wp));
	wp->address = address;
	wp->length = length;

	return g_slist_append(list, wp);

}

static GSList* clearwatchpoint(GSList* list, uint32_t address, unsigned int length) {

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

static bool checkwatchpoints(GSList* list, uint32_t address, int size) {

	GSList* iterator;
	for (iterator = list; iterator; iterator = iterator->next) {
		watchpoint* wp = iterator->data;

		int wpmin = wp->address;
		int wpmax = wp->address + (wp->length - 1);
		int actionmin = address;
		int actionmax = address + (size - 1);

		printf("Checking watchpoint 0x%08x:%d with 0x%08x:%d\n", wp->address, wp->length, address, size);

		printf("watch point range %x -> %x\n", wpmin, wpmax);
		printf("action range %x -> %x\n", actionmin, actionmax);

		if ((actionmin >= wpmin && actionmin <= wpmax) || (actionmax >= wpmin && actionmax <= wpmax)) {

			printf("ranges overlap\n");

			return true;
		}
	}

	return false;

}

void gdbserver_set_watchpoint(uint32_t address, unsigned int length, bool read, bool write) {

	if (read && write) {
		watchpoints_access = addwatchpoint(watchpoints_access, address, length);
	}

	else if (read) {
		watchpoints_read = addwatchpoint(watchpoints_read, address, length);
	}

	else if (write) {
		watchpoints_write = addwatchpoint(watchpoints_write, address, length);
	}
}

void gdbserver_clear_watchpoint(uint32_t address, unsigned int length, bool read, bool write) {

	if (read && write) {
		watchpoints_access = clearwatchpoint(watchpoints_access, address, length);
	}

	else if (read) {
		watchpoints_read = clearwatchpoint(watchpoints_read, address, length);
	}

	else if (write) {
		watchpoints_write = clearwatchpoint(watchpoints_write, address, length);
	}
}

char crap[1024];

void gdbserver_check_watchpoints(uint32_t address, uint32_t value, bool write, int size) {

	static char stopreply[256];

	if (checkwatchpoints(watchpoints_access, address, size)) {
		m68k_end_timeslice();
		log_println(LEVEL_INFO, TAG, "Access at 0x%08x;", address);
		state = WAITING;
		sprintf(stopreply, "T05awatch:%08x;", address);
		gdbserver_sendpacket(socketconnection, stopreply);
	}

	if (write) {
		if (checkwatchpoints(watchpoints_write, address, size)) {
			m68k_end_timeslice();
			log_println(LEVEL_INFO, TAG, "Write 0x%08x to 0x%08x PC[0x%08x]", value, address,
					m68k_get_reg(NULL, M68K_REG_PC));
			state = WAITING;
			sprintf(stopreply, "T05watch:%08x;", address);
			gdbserver_sendpacket(socketconnection, stopreply);
		}
	}

	else {
		if (checkwatchpoints(watchpoints_read, address, size)) {
			m68k_end_timeslice();
			log_println(LEVEL_INFO, TAG, "Read at 0x%08x; %s", address);
			state = WAITING;
			sprintf(stopreply, "T05rwatch:%08x;", address);
			gdbserver_sendpacket(socketconnection, stopreply);
		}
	}

}

char* gbdserver_munchhexstring(char* buffer) {

	static char string[256];
	int stringpos = 0;

	memset(string, 0, 256);
	unsigned int current;
	while (sscanf(buffer, "%2x", &current) > 0) {
		string[stringpos] = (char) current;
		stringpos += 1;
		buffer += 2;
	}

	return string;
}

char* gdbserver_query(char* commandbuffer) {

	char* ret = "";
	if (strncmp(commandbuffer, "qRcmd", 4) == 0) {

		char* offset = strchr(commandbuffer, ',') + 1;
		printf("GDB is sending a monitor command; %s\n", offset);
		char* monitorcommand = gbdserver_munchhexstring(offset);

		if (strncmp(monitorcommand, "load ", 5) == 0) {
			printf("User has requested that a new binary is loaded into ROM\n");
			romcard_loadrom(monitorcommand + 5, false);
			ret = "OK";
		}

		else if (strncmp(monitorcommand, "reset", 5) == 0) {
			printf("User has requested the CPU is reset\n");
			sim_reset();
			ret = "OK";
		}

		else if (strncmp(monitorcommand, "stfu", 4) == 0) {
			printf("User has requested we keep quiet\n");
			ret = "OK";
		}

		else if (strncmp(monitorcommand, "talktome", 8) == 0) {
			printf("User wants to know whats going down\n");
			ret = "OK";
		}

	}

	else {
		printf("Dunno what %s is\n", commandbuffer);
	}

	return ret;
}

char* gbdserver_readregs(char* commandbuffer) {

	return getregistersstring(m68k_get_reg(NULL, M68K_REG_D0), m68k_get_reg(NULL, M68K_REG_D1),
			m68k_get_reg(NULL, M68K_REG_D2), m68k_get_reg(NULL, M68K_REG_D3), m68k_get_reg(NULL, M68K_REG_D4),
			m68k_get_reg(NULL, M68K_REG_D5), m68k_get_reg(NULL, M68K_REG_D6), m68k_get_reg(NULL, M68K_REG_D7),
			m68k_get_reg(NULL, M68K_REG_A0), m68k_get_reg(NULL, M68K_REG_A1), m68k_get_reg(NULL, M68K_REG_A2),
			m68k_get_reg(NULL, M68K_REG_A3), m68k_get_reg(NULL, M68K_REG_A4), m68k_get_reg(NULL, M68K_REG_A5),
			m68k_get_reg(NULL, M68K_REG_A6), // fp
			m68k_get_reg(NULL, M68K_REG_A7), // sp
			m68k_get_reg(NULL, M68K_REG_SR), // ps
			m68k_get_reg(NULL, M68K_REG_PC));
}

static char* readmem(char* commandbuffer) {
	char *address = strtok(&commandbuffer[1], "m,#");
	char *size = strtok(NULL, "m,#");

	unsigned int ad = strtoul(address, NULL, 16);
	int sz = strtol(size, NULL, 16);
//printf("address %s, %d , size %s, %d", address, ad, size, sz);

	return getmemorystring(ad, sz);
}

void gdbserver_instruction_hook_callback() {
	uint32_t pc = m68k_get_reg(NULL, M68K_REG_PC);
	gdbserver_check_breakpoints(pc);
	profiler_onpcchange(pc);
}

void gdbserver_check_breakpoints(uint32_t pc) {

	if (state == STEPPING) {
		m68k_end_timeslice();
		m68k_disassemble(crap, pc, M68K_CPU_TYPE_68000);
		log_println(LEVEL_INFO, TAG, "Stepped to 0x%08x; %s", pc, crap);
		state = BREAKING;
	}

	GSList* iterator;
	for (iterator = breakpoints; iterator; iterator = iterator->next) {
		if (GPOINTER_TO_UINT(iterator->data) == pc) {
			m68k_end_timeslice();
			m68k_disassemble(crap, pc, M68K_CPU_TYPE_68000);
			log_println(LEVEL_INFO, TAG, "Broke at 0x%08x; %s", pc, crap);
			state = WAITING;
			gdbserver_sendpacket(socketconnection, STOP_WEBROKE);
			break;
		}
	}
}

void gdbserver_setport(int port) {
	struct sockaddr_in servaddr;
	if ((socketlistening = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
		log_println(LEVEL_WARNING, TAG, "Failed to create socket");
		//return 1;
	}

	memset(&servaddr, 0, sizeof(servaddr));
	servaddr.sin_family = AF_INET;
	servaddr.sin_addr.s_addr = htonl(INADDR_LOOPBACK);
	servaddr.sin_port = htons(port);

	if (bind(socketlistening, (struct sockaddr *) &servaddr, sizeof(servaddr)) < 0) {
		log_println(LEVEL_WARNING, TAG, "bind() failed");
		state = EXIT;
		return;
	}

	if (listen(socketlistening, 0) < 0) {
		log_println(LEVEL_WARNING, TAG, "listen() failed");
		state = EXIT;
		return;
	}

	log_println(LEVEL_INFO, TAG, "Listening for GDB connection on %d", port);
}

void gdb_hitstop() {
	log_println(LEVEL_INFO, TAG, "hit stop");
	cpu_pulse_stop();
}

void gdb_onpcmodified(uint32_t a) {

	if (state == RUNNING) {
		profiler_onpcmodified(m68k_get_reg(NULL, M68K_REG_PPC), a);
	}

}

void gdb_break() {
	state = BREAKING;
}

uint8_t gdbserver_m68k_read_byte(uint32_t address) {
	gdbserver_check_watchpoints(address, 0, false, 1);
	return board_read_byte(address);
}

uint16_t gdbserver_m68k_read_word(uint32_t address) {
	gdbserver_check_watchpoints(address, 0, false, 2);
	return board_read_word(address);
}

uint32_t gdbserver_m68k_read_long(uint32_t address) {
	gdbserver_check_watchpoints(address, 0, false, 4);
	return board_read_long(address);
}

void gdbserver_m68k_write_byte(uint32_t address, uint8_t value) {
	gdbserver_check_watchpoints(address, value, true, 1);
	return board_write_byte(address, value);
}

void gdbserver_m68k_write_word(uint32_t address, uint16_t value) {
	gdbserver_check_watchpoints(address, value, true, 2);
	return board_write_word(address, value);
}

void gdbserver_m68k_write_long(uint32_t address, uint32_t value) {
	gdbserver_check_watchpoints(address, value, true, 4);
	return board_write_long(address, value);
}
