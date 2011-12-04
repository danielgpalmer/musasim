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

#include "common.h"
#include "sim.h"
#include "m68k.h"
#include "gdbserver.h"

#include "hardware/cards/romcard.h"

void cleanup();

GSList* breakpoints;

int socketlistening;
int socketconnection;

State state = LISTENING;

bool verbose = true;

int port;
char* endpointer;

bool parseargs(int argc, char* argv[]) {

	if (argc == 2 || argc == 3) {

		if (argc == 3) {
			if(romcard_loadrom(argv[2])){
				// TODO Insert error handling here
			}
		}

		port = strtol(argv[1], &endpointer, 0);
		if (*endpointer) {
			printf("Invalid port number");
			return false;
		}

		if (port < 1024) {
			printf("Port number is smaller than 1024\n");
			return false;
		}

	}

	else {
		printf("usage: %s <port> [rom binary]\n", argv[0]);
		return false;
	}

	return true;
}

int main(int argc, char* argv[]) {

	int steps = 0;

	printf("musashi m68k emulator\tKarl Stenerud with patches from MAME up to 0105\n"
			"gdbserver for musashi\tDaniel Palmer (daniel@0x0f.com)\n");

	struct sockaddr_in servaddr;

	if (!parseargs(argc, argv)) {
		exit(EXIT_FAILURE);
	}

	if ((socketlistening = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
		fprintf(stderr, "Failed to create socket\n");
		exit(EXIT_FAILURE);
	}


	memset(&servaddr, 0, sizeof(servaddr));
	servaddr.sin_family = AF_INET;
	servaddr.sin_addr.s_addr = htonl(INADDR_LOOPBACK);
	servaddr.sin_port = htons(port);

	if (bind(socketlistening, (struct sockaddr *) &servaddr, sizeof(servaddr)) < 0) {
		fprintf(stderr, "bind() failed\n");
		exit(EXIT_FAILURE);
	}

	if (listen(socketlistening, 0) < 0) {
		fprintf(stderr, "listen() failed\n");
		exit(EXIT_FAILURE);
	}

	sim_init();

	printf("Registering signal handlers\n");
	registersighandler();

	printf("Resetting CPU\n");
	simreset();

	while (state != EXIT) {

		switch (state) {

			case LISTENING:
				printf("Listening for GDB connection on %d\n", port);

				if ((socketconnection = accept(socketlistening, NULL, NULL)) > 0) {
					printf("Got connection from GDB\n");

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
				readcommand(socketconnection);
				break;

			case RUNNING:
				if (isbreakpoint(m68k_get_reg(NULL, M68K_REG_PC))) {
					state = WAITING;
					sendpacket(socketconnection, "S05");
				}
				else {
					steps += 1;

					simstep();
					if (steps > 800000) {
						simtick();
						steps = 0;
					}
					//usleep(1);
				}
				break;

			case BREAKING:
				sendpacket(socketconnection, "S05"); // alert GDB to the fact that execution has stopped
				state = WAITING;
				break;

			default:
				break;
		}
	};

	cleanup();
	exit(EXIT_SUCCESS);

}

void readcommand(int s) {

	State newstate = WAITING;

	static char inputbuffer[MAXPACKETLENGTH];
	static char res[] = { '+', '-' };

	memset(inputbuffer, 0, MAXPACKETLENGTH);
	gdbread(s, inputbuffer);
	if (verbose) {
		printf("<-- %s\n", inputbuffer);
	}

	char* data = "";

	char command = inputbuffer[0];
	switch (command) {
		case 'g':
			data = readregs(inputbuffer);
			break;
		case 'G':
			if (verbose) {
				printf("GDB wants to write the registers\n");
			}
			data = OK;
			break;
		case 'p':
			if (verbose) {
				printf("GDB wants to read a single register\n");
			}
			data = "00000000";
			break;
		case 'P':
			if (verbose) {
				printf("GDB wants to write a single register\n");
			}
			break;
		case 'm':
			data = readmem(inputbuffer);
			break;
		case 'M':
			if (verbose) {
				printf("GDB wants to write to memory\n");
			}
			data = "OK";

			break;
		case 'c':
			if (verbose) {
				printf("GDB wants execution to continue\n");
			}
			data = "OK";
			newstate = RUNNING;
			break;
		case 's':
			if (verbose) {
				printf("GDB wants execution to step\n");
			}
			simstep();
			data = "S05";
			break;
		case '?':
			if (verbose) {
				printf("GDB wants to know why we halted\n");
			}
			data = "S05";
			break;
		case 'r':
			if (verbose) {
				printf("GDB wants the processor to reset\n");
			}
			simreset();
			data = "OK";
			break;

		case 'q':
			data = query(inputbuffer);
			break;

		case 'D':
			printf("GDB is detaching!\n");
			data = "OK";
			newstate = LISTENING;
			break;

		case 'k':
			printf("GDB killed us\n");
			data = "OK";
			newstate = LISTENING;
			break;

		case 'z':
			printf("GDB is unsetting a breakpoint\n");
			strtok(inputbuffer, "Z,#");
			char *xbreakaddress = strtok(NULL, "Z,#");
			strtok(NULL, "Z,#");

			printf("0x%s\n", xbreakaddress);

			clearbreakpoint(strtoul(xbreakaddress, NULL, 16));
			data = OK;

			break;

		case 'Z':
			if (verbose) {
				printf("GDB is setting a breakpoint at ");
			}
			strtok(inputbuffer, "Z,#");
			char *breakaddress = strtok(NULL, "Z,#");
			strtok(NULL, "Z,#");

			if (verbose) {
				printf("0x%s\n", breakaddress);
			}
			setbreakpoint(strtoul(breakaddress, NULL, 16));
			data = "OK";
			break;

		default:
			if (verbose) {
				fprintf(stderr, "Command %c is unknown\n", command);
			}
			break;
	}

	gdbwrite(s, &res[0], 1);

	if (sendpacket(s, data)) {
		state = newstate;
	}
	else {
		close(s);
		state = LISTENING;
	}

}

#define MAXSENDTRIES 10

bool sendpacket(int s, char* data) {

	int triesleft = MAXSENDTRIES;
	char res;
	static char outputbuffer[MAXPACKETLENGTH];
	memset(outputbuffer, 0, MAXPACKETLENGTH);
	static int outputlen;

	outputlen = sprintf(outputbuffer, "$%s#", data);

	outputlen += sprintf(outputbuffer + outputlen, "%02x", calcchecksum(data));

	while (res != '+') {
		gdbwrite(s, outputbuffer, outputlen);
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

void gdbwrite(int s, char* buffer, int len) {

	if (verbose) {
		printf("--> \"");
		int i;
		for (i = 0; i < len; i++) {
			printf("%c", buffer[i]);
		}
		printf("\"\n");
	}

	write(s, buffer, len);
}

void gdbread(int s, char *buffer) {

	ReadState readstate = WAITINGFORSTART;
	int bytessofar = 0;
	int bufferpos = 0;

	while (readstate != DONE) {

		static char readbuffer[32];
		//printf("pulling from socket\n");
		bytessofar += read(s, readbuffer + bytessofar, 32);

		//printf("%d total, at %d, state %d\n", bytessofar, bufferpos, readstate);

		switch (readstate) {
			case WAITINGFORSTART:
				//printf("waiting for start\n");
				for (; bufferpos < bytessofar; bufferpos++) {
					if (readbuffer[bufferpos] == '$') {
						//printf("Got packet start at %d\n", bufferpos);
						readstate = READINGPACKET;
						break;
					}
				}

				if (readstate == WAITINGFORSTART || bufferpos == bytessofar) {
					break;
				}
				else {
					bufferpos++;
				}

			case READINGPACKET:
				for (; bufferpos < bytessofar; bufferpos++) {
					if (readbuffer[bufferpos] == '#') {
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

				//printf("checkone %c\n", readbuffer[bufferpos]);
				bufferpos++;

				readstate = CHECKSUMDIGITTWO;

			case CHECKSUMDIGITTWO:

				if (bufferpos > bytessofar) {
					break;
				}

				//printf("checktwo %c\n", readbuffer[bufferpos]);

				readstate = DONE;
			case DONE:
				break;
		}
	}

}

int calcchecksum(char *data) {

	int len = strlen(data);

	int checksum = 0;
	int i;
	for (i = 0; i < len; i++) {
		checksum += data[i];

	}
	checksum = checksum & 0xFF;

	return checksum;
}

void cleanup() {
	printf("Cleaning up\n");
	sim_quit();
	close(socketlistening);
	close(socketconnection);
	g_slist_free(breakpoints);
}

void request_exit(){
	printf("Exit requested\n");
	state = EXIT;
	kill(0, SIGINT);
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

char* getregistersstring(int d0, int d1, int d2, int d3, int d4, int d5, int d6, int d7, int a0, int a1, int a2, int a3,
		int a4, int a5, int fp, int sp, int ps, int pc) {

	static char registersstring[168];
	memset(registersstring, 0, 168);

	sprintf(registersstring, "%08x%08x%08x%08x%08x%08x%08x%08x%08x%08x%08x%08x%08x%08x%08x%08x%08x%08x", d0, d1, d2, d3,
			d4, d5, d6, d7, a0, a1, a2, a3, a4, a5, fp, sp, ps, pc);

	return registersstring;

}

char* getmemorystring(unsigned int address, int len) {

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

void setbreakpoint(uint32_t address) {

	printf("Setting break at 0x%x\n", address);
	breakpoints = g_slist_append(breakpoints, GUINT_TO_POINTER(address));

}

void clearbreakpoint(uint32_t address) {
	printf("Clearing break at 0x%x\n", address);
	breakpoints = g_slist_remove(breakpoints, GUINT_TO_POINTER(address));

}

bool isbreakpoint(uint32_t address) {
	GSList* iterator;
	for (iterator = breakpoints; iterator; iterator = iterator->next) {
		if (GPOINTER_TO_UINT(iterator->data) == address) {
			printf("BREAK!\n");
			return true;
		}
	}
	return false;
}

char* munchhexstring(char* buffer) {

	static char string[256];
	int stringpos = 0;

	memset(string, 0, 256);
	unsigned int current;
	while (sscanf(buffer, "%2x", &current) > 0) {
		string[stringpos] = (char) current;
		stringpos += 1;
		buffer += 2;
	}
	printf("%s\n", string);

	return string;

}

char* query(char* commandbuffer) {

	char* ret = "";

	if (verbose) {
		printf("GDB is querying something\n");
	}
	int result = strncmp(commandbuffer, "qRcmd", 4);
	if (result == 0) {

		char* offset = strchr(commandbuffer, ',') + 1;
		printf("GDB is sending a monitor command; %s\n", offset);
		char* monitorcommand = munchhexstring(offset);

		if (strncmp(monitorcommand, "load ", 5) == 0) {
			printf("User has requested that a new binary is loaded into ROM\n");
			romcard_loadrom(monitorcommand + 5);
			ret = "OK";
		}

		else if (strncmp(monitorcommand, "reset", 5) == 0) {
			printf("User has requested the CPU is reset\n");
			simreset();
			ret = "OK";
		}

		else if (strncmp(monitorcommand, "stfu", 4) == 0) {
			printf("User has requested we keep quiet\n");
			verbose = false;
			ret = "OK";
		}

		else if (strncmp(monitorcommand, "talktome", 8) == 0) {
			printf("User wants to know whats going down\n");
			verbose = true;
			ret = "OK";
		}

	}

	return ret;
}

char* readregs(char* commandbuffer) {
	if (verbose) {
		printf("GDB wants to read the registers\n");
	}
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

char* readmem(char* commandbuffer) {
	if (verbose) {
		printf("GDB wants to read from memory\n");
	}

	char *address = strtok(&commandbuffer[1], "m,#");
	char *size = strtok(NULL, "m,#");

	unsigned int ad = strtoul(address, NULL, 16);
	int sz = strtol(size, NULL, 16);
	//printf("address %s, %d , size %s, %d", address, ad, size, sz);

	return getmemorystring(ad, sz);
}




