#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <stdbool.h>
#include <stdint.h>
#include <time.h>
#include <glib.h>

#include "sim.h"
#include "m68k.h"

#include "hardware/board.h"
#include "hardware/video.h"

/* Memory-mapped IO ports */
#define INPUT_ADDRESS 0x800000
#define OUTPUT_ADDRESS 0x400000

/* IRQ connections */
#define IRQ_NMI_DEVICE 7
#define IRQ_INPUT_DEVICE 2
#define IRQ_OUTPUT_DEVICE 1

/* Time between characters sent to output device (seconds) */
#define OUTPUT_DEVICE_PERIOD 1



/* Prototypes */
void exit_error(char* fmt, ...);
int osd_get_char(void);


void cpu_pulse_reset(void);
void cpu_set_fc(unsigned int fc);
int cpu_irq_ack(int level);

void nmi_device_reset(void);
void nmi_device_update(void);
int nmi_device_ack(void);

void input_device_reset(void);
void input_device_update(void);
int input_device_ack(void);
unsigned int input_device_read(void);
void input_device_write(unsigned int value);

void output_device_reset(void);
void output_device_update(void);
int output_device_ack(void);
unsigned int output_device_read(void);
void output_device_write(unsigned int value);

void int_controller_set(unsigned int value);
void int_controller_clear(unsigned int value);

void get_user_input(void);



void badread(unsigned int address);
void badwriterom(unsigned int address);
void badwrite(unsigned int address);

/* Data */
unsigned int g_quit = 0; /* 1 if we want to quit */
unsigned int g_nmi = 0; /* 1 if nmi pending */

int g_input_device_value = -1; /* Current value in input device */

unsigned int g_output_device_ready = 0; /* 1 if output device is ready */
time_t g_output_device_last_output; /* Time of last char output */

unsigned int g_int_controller_pending = 0; /* list of pending interrupts */
unsigned int g_int_controller_highest_int = 0; /* Highest pending interrupt */

/* Exit with an error message.  Use printf syntax. */
void exit_error(char* fmt, ...)
{
	va_list args;
	va_start(args, fmt);
	vfprintf(stderr, fmt, args);
	va_end(args);
	fprintf(stderr, "\n");

	exit(EXIT_FAILURE);
}

/* OS-dependant code to get a character from the user.
 * This function must not block, and must either return an ASCII code or -1.
 */
//#include <conio.h>
int osd_get_char(void) {
	int ch = -1;
	//if(kbhit())
	//{
	//	while(kbhit())
	//		ch = getch();
	//}

	ch = getchar();

	printf("here\n");

	return ch;
}



/* Called when the CPU pulses the RESET line */
void cpu_pulse_reset(void) {
	nmi_device_reset();
	output_device_reset();
	input_device_reset();
}

/* Called when the CPU changes the function code pins */
void cpu_set_fc(unsigned int fc) {
	g_fc = fc;
}

/* Called when the CPU acknowledges an interrupt */
int cpu_irq_ack(int level) {
	switch (level) {
	case IRQ_NMI_DEVICE:
		return nmi_device_ack();
	case IRQ_INPUT_DEVICE:
		return input_device_ack();
	case IRQ_OUTPUT_DEVICE:
		return output_device_ack();
	}
	return M68K_INT_ACK_SPURIOUS;
}

/* Implementation for the NMI device */
void nmi_device_reset(void) {
	g_nmi = 0;
}

void nmi_device_update(void) {
	if (g_nmi) {
		g_nmi = 0;
		int_controller_set(IRQ_NMI_DEVICE);
	}
}

int nmi_device_ack(void) {
	printf("\nNMI\n");
	fflush(stdout);
	int_controller_clear(IRQ_NMI_DEVICE);
	return M68K_INT_ACK_AUTOVECTOR;
}

/* Implementation for the input device */
void input_device_reset(void) {
	g_input_device_value = -1;
	int_controller_clear(IRQ_INPUT_DEVICE);
}

void input_device_update(void) {
	if (g_input_device_value >= 0)
		int_controller_set(IRQ_INPUT_DEVICE);
}

int input_device_ack(void) {
	return M68K_INT_ACK_AUTOVECTOR;
}

unsigned int input_device_read(void) {
	int value = g_input_device_value > 0 ? g_input_device_value : 0;
	int_controller_clear(IRQ_INPUT_DEVICE);
	g_input_device_value = -1;
	return value;
}

void input_device_write(unsigned int value) {
}

/* Implementation for the output device */
void output_device_reset(void) {
	g_output_device_last_output = time(NULL);
	g_output_device_ready = 0;
	int_controller_clear(IRQ_OUTPUT_DEVICE);
}

void output_device_update(void) {
	if (!g_output_device_ready) {
		if ((time(NULL) - g_output_device_last_output) >= OUTPUT_DEVICE_PERIOD)
		{
			g_output_device_ready = 1;
			int_controller_set(IRQ_OUTPUT_DEVICE);
		}
	}
}

int output_device_ack(void) {
	return M68K_INT_ACK_AUTOVECTOR;
}

unsigned int output_device_read(void) {
	int_controller_clear(IRQ_OUTPUT_DEVICE);
	return 0;
}

void output_device_write(unsigned int value) {
	char ch;
	if (g_output_device_ready) {
		ch = value & 0xff;
		printf("%c", ch);
		g_output_device_last_output = time(NULL);
		g_output_device_ready = 0;
		int_controller_clear(IRQ_OUTPUT_DEVICE);
	}
}

/* Implementation for the interrupt controller */
void int_controller_set(unsigned int value) {
	unsigned int old_pending = g_int_controller_pending;

	g_int_controller_pending |= (1 << value);

	if (old_pending != g_int_controller_pending && value > g_int_controller_highest_int) {
		g_int_controller_highest_int = value;
		m68k_set_irq(g_int_controller_highest_int);
	}
}

void int_controller_clear(unsigned int value) {
	g_int_controller_pending &= ~(1 << value);

	for (g_int_controller_highest_int = 7; g_int_controller_highest_int > 0; g_int_controller_highest_int--)
		if (g_int_controller_pending & (1 << g_int_controller_highest_int))
			break;

	m68k_set_irq(g_int_controller_highest_int);
}

/* Parse user input and update any devices that need user input */
void get_user_input(void) {
	static int last_ch = -1;
	int ch = osd_get_char();

	if (ch >= 0) {
		switch (ch) {
		case 0x1b:
			g_quit = 1;
			break;
		case '~':
			if (last_ch != ch)
				g_nmi = 1;
			break;
		default:
			g_input_device_value = ch;
		}
	}
	last_ch = ch;
}

void sim_init() {

	printf("sim_init()\n");
	g_thread_init(NULL);
	video_init();
}

void sim_quit(){

	printf("sim_quit()\n");
	video_quit();

}

void simtick() {
	video_tick();
}

void simreset() {
	m68k_pulse_reset();
	input_device_reset();
	output_device_reset();
	nmi_device_reset();
}

void simstep() {
	m68k_execute(4);
	//output_device_update();
	//input_device_update();
	//nmi_device_update();
}

/* The main loop */

//int main(int argc, char* argv[])
//{
//	FILE* fhandle;
//
//	if(argc != 2)
//		exit_error("Usage: sim <program file>");
//
//	if((fhandle = fopen(argv[1], "rb")) == NULL)
//		exit_error("Unable to open %s", argv[1]);
//
//	if(fread(g_rom, 1, MAX_ROM+1, fhandle) <= 0)
//		exit_error("Error reading %s", argv[1]);
//
//
//	m68k_pulse_reset();
//	input_device_reset();
//	output_device_reset();
//	nmi_device_reset();
//
//	g_quit = 0;
//	while(!g_quit)
//	{
//		get_user_input();
//		/* Note that I am not emulating the correct clock speed! */
//		m68k_execute(1000);
//		output_device_update();
//		input_device_update();
//		nmi_device_update();
//	}
//	return 0;
//}

