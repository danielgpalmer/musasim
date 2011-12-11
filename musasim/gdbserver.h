#ifndef GDB__HEADER
#define GDB__HEADER

#define GDBACK '+';
#define GDBNAK '-';

#define MAXPACKETLENGTH 256

void readcommand(int s);
void cleanup();
void request_exit();
void termination_handler(int signum);
void registersighandler();
void gdbwrite(int s, char* buffer, int len);
void gdbread(int s, char *buffer);
bool sendpacket(int s, char* data);
int calcchecksum(char *data);
char* getregistersstring(int d0, int d1, int d2, int d3, int d4, int d5, int d6, int d7, int a0, int a1, int a2, int a3,
		int a4, int a5, int fp, int sp, int ps, int pc);
char* getmemorystring(unsigned int address, int len);

void gbserver_set_breakpoint(uint32_t address);
void gdbserver_clear_breakpoint(uint32_t address);
bool isbreakpoint(uint32_t address);
char* gdbserver_query(char* commandbuffer);
char* gbdserver_readregs(char* commandbuffer);
char* readmem(char* commandbuffer);

void gdbserver_check_breakpoints();

typedef enum State {
	LISTENING, WAITING, RUNNING, BREAKING, EXIT

} State;

typedef enum ReadState {
	WAITINGFORSTART, READINGPACKET, CHECKSUMDIGITONE, CHECKSUMDIGITTWO, DONE
} ReadState;

#endif
