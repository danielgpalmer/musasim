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
#include "hardware/cards/romcard.h"
#include "hardware/cards/videocard.h"
#include "hardware/cards/uartcard.h"
#include "hardware/cards/soundcard.h"
#include "hardware/cards/compactflashinterfacecard.h"
#include "hardware/cards/inputcard.h"
#include "hardware/cards/dmacard.h"

void cpu_pulse_reset(void) {

}

int cpu_irq_ack(int level) {
	return 0;
}

/* Called when the CPU changes the function code pins */
void cpu_set_fc(unsigned int fc) {
	g_fc = fc;
}

void sim_init() {

	printf("sim_init()\n");
	g_thread_init(NULL);

	board_add_device(0, &romcard);
	board_add_device(1, &videocard);
	board_add_device(2, &uartcard);
	board_add_device(3, &soundcard);
	board_add_device(4, &compactflashinterfacecard);
	board_add_device(5, &inputcard);
	board_add_device(6, &dmacard);

}

void sim_tick() {
	if (!board_bus_locked()) {
		m68k_execute(4);
	}
	board_tick();
}

void sim_quit() {

	printf("sim_quit()\n");
	board_poweroff();

}

void sim_reset() {
	m68k_pulse_reset();
}

//void simstep() {
//output_device_update();
//input_device_update();
//nmi_device_update();
//}

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
