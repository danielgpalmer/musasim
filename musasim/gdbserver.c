#include <sys/socket.h>
#include <sys/types.h>
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

#include <glib.h>

#include "sim.h"
#include "musashi/m68k.h"
#include "args.h"

#include "hardware/cards/romcard.h"
#include "logging.h"

#include "gdbserver.h"

// the overall state of the program
typedef enum State {
	LISTENING, WAITING, RUNNING, BREAKING, EXIT

} State;

// the state of the packet reader
typedef enum ReadState {
	WAITINGFORSTART, READINGPACKET, CHECKSUMDIGITONE, CHECKSUMDIGITTWO, DONE
} ReadState;

// some constants for the GDB proto
static char OK[] = "OK";
static char GDBACK = '+';
static char GDBNAK = '-';
#define GDBPACKETSTART '$'
#define GDBDATAEND '#'
static char WEBROKE[] = "S05";

#define MAXPACKETLENGTH 256

static void gdbserver_cleanup();
static void termination_handler(int signum);
static void registersighandler();

// tcp/ip connection related stuff
static int port;
static int socketlistening;
static int socketconnection;

// stuff that talks to gdb
static void gdbserver_readcommand(int s);
static bool gdbserver_readpacket(int s, char *buffer);
static bool gdbserver_sendpacket(int s, char* data);
static char* gdbserver_query(char* commandbuffer);

// breakpoint stuff
static GSList* breakpoints;
static void gbserver_set_breakpoint(uint32_t address);
static void gdbserver_clear_breakpoint(uint32_t address);

// stuff that pokes the sim
static char* readmem(char* commandbuffer);

// utils
static char* getmemorystring(unsigned int address, int len);
static char* gbdserver_readregs(char* commandbuffer);
static int gdbserver_calcchecksum(char *data);
static char* getregistersstring(int d0, int d1, int d2, int d3, int d4, int d5, int d6, int d7, int a0, int a1, int a2,
		int a3, int a4, int a5, int fp, int sp, int ps, int pc);

static State state = LISTENING;

static const char TAG[] = "gdbserver";

int main(int argc, char* argv[]) {

	log_println(LEVEL_INFO, TAG, "musashi m68k emulator\tKarl Stenerud with patches from MAME up to 0105");
	log_println(LEVEL_INFO, TAG, "gdbserver for musashi\tDaniel Palmer (daniel@0x0f.com)");

	struct sockaddr_in servaddr;

	if (!args_parse(argc, argv)) {
		return 0;
	}

	if ((socketlistening = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
		log_println(LEVEL_WARNING, TAG, "Failed to create socket");
		return 1;
	}

	memset(&servaddr, 0, sizeof(servaddr));
	servaddr.sin_family = AF_INET;
	servaddr.sin_addr.s_addr = htonl(INADDR_LOOPBACK);
	servaddr.sin_port = htons(port);

	if (bind(socketlistening, (struct sockaddr *) &servaddr, sizeof(servaddr)) < 0) {
		log_println(LEVEL_WARNING, TAG, "bind() failed");
		return 1;
	}

	if (listen(socketlistening, 0) < 0) {
		log_println(LEVEL_WARNING, TAG, "listen() failed");
		return 1;
	}

	sim_init();
	registersighandler();
	sim_reset();

	while (state != EXIT) {

		switch (state) {

			case LISTENING:
				log_println(LEVEL_INFO, TAG, "Listening for GDB connection on %d", port);

				if ((socketconnection = accept(socketlistening, NULL, NULL)) > 0) {
					log_println(LEVEL_INFO, TAG, "Got connection from GDB");

					fcntl(socketconnection, F_SETOWN, getpid());
					int flags = fcntl(socketconnection, F_GETFL, 0);
					fcntl(socketconnection, F_SETFL, flags | FASYNC);
					state = WAITING;
				}
				else {
					printf("YAR!\n");
				}
				break;

			case WAITING:
				gdbserver_readcommand(socketconnection);
				break;

			case RUNNING:
				printf("--tick --\n");
				sim_tick();
				if (sim_has_quit()) {
					state = EXIT;
				}
				break;

			case BREAKING:
				gdbserver_sendpacket(socketconnection, WEBROKE); // alert GDB to the fact that execution has stopped
				state = WAITING;
				break;

			default:
				break;
		}
	};

	gdbserver_cleanup();
	return 0;

}

static bool step = false;

static void gdbserver_readcommand(int s) {

	State newstate = WAITING;
	static char inputbuffer[MAXPACKETLENGTH];
	memset(inputbuffer, 0, MAXPACKETLENGTH);

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
				printf("GDB wants to read a single register\n");
				data = "00000000";
				break;
			case 'P':
				printf("GDB wants to write a single register\n");
				break;
			case 'm':
				printf("GDB wants to read from memory\n");
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
				step = true;
				sim_tick(); // FIXME check if this actually works

				break;
			case '?':
				printf("GDB wants to know why we halted\n");
				data = WEBROKE;
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

			case 'z': {
				strtok(inputbuffer, "Z,#");
				char *breakaddress = strtok(NULL, "Z,#");
				strtok(NULL, "Z,#");

				gdbserver_clear_breakpoint(strtoul(breakaddress, NULL, 16));

				printf("GDB is unsetting a breakpoint at 0x%s\n", breakaddress);

			}
				break;

			case 'Z': {
				strtok(inputbuffer, "Z,#");
				char *breakaddress = strtok(NULL, "Z,#");
				strtok(NULL, "Z,#");

				gbserver_set_breakpoint(strtoul(breakaddress, NULL, 16));

				printf("GDB is setting a breakpoint at 0x%s\n", breakaddress);

			}
				break;

			default:

				fprintf(stderr, "Command %c is unknown\n", command);
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
			printf("Sent packet %d times, giving up!\n", MAXSENDTRIES);
			return false;
		}
	}
	return true;
}

static bool gdbserver_readpacket(int s, char *buffer) {

	ReadState readstate = WAITINGFORSTART;
	static char readbuffer[MAXPACKETLENGTH];
	//static char checksum[3];
	int bytessofar = 0;
	int bufferpos = 0;
	//int check = 0;

	while (readstate != DONE) {
		bytessofar += read(s, readbuffer + bytessofar, MAXPACKETLENGTH);
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
	printf("Cleaning up\n");
	sim_quit();
	shutdown(socketconnection, SHUT_RDWR);
	close(socketlistening);
	close(socketconnection);
	g_slist_free(breakpoints);
}

void termination_handler(int signum) {

	printf("Caught interrupt\n");
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

#ifdef DEBUG
	printf("Setting break at 0x%x\n", address);
#endif
	breakpoints = g_slist_append(breakpoints, GUINT_TO_POINTER(address));
}

void gdbserver_clear_breakpoint(uint32_t address) {

#ifdef DEBUG
	printf("Clearing break at 0x%x\n", address);
#endif
	breakpoints = g_slist_remove(breakpoints, GUINT_TO_POINTER(address));

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
	int result = strncmp(commandbuffer, "qRcmd", 4);
	if (result == 0) {

		char* offset = strchr(commandbuffer, ',') + 1;
		printf("GDB is sending a monitor command; %s\n", offset);
		char* monitorcommand = gbdserver_munchhexstring(offset);

		if (strncmp(monitorcommand, "load ", 5) == 0) {
			printf("User has requested that a new binary is loaded into ROM\n");
			romcard_loadrom(monitorcommand + 5);
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

void gdbserver_check_breakpoints() {

	if (step) {
		m68k_end_timeslice();
		step = false;
		gdbserver_sendpacket(socketconnection, WEBROKE);
	}

	uint32_t address = m68k_get_reg(NULL, M68K_REG_PC);

#ifdef DEBUG
	printf("gdbserver_check_breakpoints(0x%x)\n", address);
#endif

	GSList* iterator;
	for (iterator = breakpoints; iterator; iterator = iterator->next) {
		if (GPOINTER_TO_UINT(iterator->data) == address) {
			printf("*** breaking at 0x%x ***\n", address);
			state = WAITING;
			gdbserver_sendpacket(socketconnection, WEBROKE);
			m68k_end_timeslice();
			break;
		}
	}
}

void gdbserver_setport(int p) {
	port = p;
}
