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

bool shouldexit = false;

void cpu_pulse_reset(void) {

}

/* Called when the CPU changes the function code pins */
void cpu_set_fc(unsigned int fc) {
	g_fc = fc;
}

void sim_init() {

	printf("sim_init()\n");
	g_thread_init(NULL);

	board_add_device(SLOT_ROMCARD, &romcard);
	board_add_device(SLOT_VIDEOCARD, &videocard);
	board_add_device(SLOT_UARTCARD, &uartcard);
	board_add_device(SLOT_SOUNDCARD, &soundcard);
	board_add_device(SLOT_CFCARD, &compactflashinterfacecard);
	board_add_device(SLOT_DMACARD, &dmacard);
	board_add_device(SLOT_INPUTCARD, &inputcard);

}

void sim_tick() {

	if (shouldexit) {
		return;
	}

	if (!board_bus_locked()) {
		m68k_execute(4);
	}
	board_tick();
}

void sim_quit() {

	printf("sim_quit()\n");
	board_poweroff();
	m68k_end_timeslice();
	shouldexit = true;

}

void sim_reset() {
	m68k_pulse_reset();
}

bool sim_has_quit() {
	return shouldexit;
}
