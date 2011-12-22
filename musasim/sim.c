#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <stdbool.h>
#include <stdint.h>
#include <time.h>
#include <glib.h>

#include <sys/time.h>

#include "sim.h"
#include "logging.h"
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

static const char TAG[] = "sim";

void cpu_pulse_reset(void) {

}

/* Called when the CPU changes the function code pins */
void cpu_set_fc(unsigned int fc) {
	g_fc = fc;
}

void sim_init() {

	log_println(LEVEL_DEBUG, TAG, "sim_init()");
	g_thread_init(NULL);

	board_add_device(SLOT_ROMCARD, &romcard);
	board_add_device(SLOT_VIDEOCARD, &videocard);
	board_add_device(SLOT_UARTCARD, &uartcard);
	board_add_device(SLOT_SOUNDCARD, &soundcard);
	board_add_device(SLOT_CFCARD, &compactflashinterfacecard);
	board_add_device(SLOT_DMACARD, &dmacard);
	board_add_device(SLOT_INPUTCARD, &inputcard);

}

/* Subtract the `struct timeval' values X and Y,
 storing the result in RESULT.
 Return 1 if the difference is negative, otherwise 0.  */

int timeval_subtract(result, x, y)
	struct timeval *result, *x, *y; {
	/* Perform the carry for the later subtraction by updating y. */
	if (x->tv_usec < y->tv_usec) {
		int nsec = (y->tv_usec - x->tv_usec) / 1000000 + 1;
		y->tv_usec -= 1000000 * nsec;
		y->tv_sec += nsec;
	}
	if (x->tv_usec - y->tv_usec > 1000000) {
		int nsec = (x->tv_usec - y->tv_usec) / 1000000;
		y->tv_usec += 1000000 * nsec;
		y->tv_sec -= nsec;
	}

	/* Compute the time remaining to wait.
	 tv_usec is certainly positive. */
	result->tv_sec = x->tv_sec - y->tv_sec;
	result->tv_usec = x->tv_usec - y->tv_usec;

	/* Return 1 if result is negative. */
	return x->tv_sec < y->tv_sec;
}

void sim_tick() {

	static struct timeval start, end, diff;
	static long int lastoutput = 0;
	static long int average = 0;

	if (shouldexit) {
		return;
	}

	gettimeofday(&start, NULL);

	if (!board_bus_locked()) {
		m68k_execute(4);
	}
	board_tick();

	gettimeofday(&end, NULL);

	timeval_subtract(&diff, &end, &start);

	average = (average + diff.tv_usec) / 2;

	if(diff.tv_sec > 0){
		log_println(LEVEL_DEBUG, TAG, "tick took longer than one second");
	}

	if (lastoutput == 0) {
		lastoutput = end.tv_sec;
	}

	if (lastoutput < (end.tv_sec - 10)) {
		lastoutput = end.tv_sec;
		log_println(LEVEL_DEBUG, TAG, "tick is taking %d us", average);
	}

}

void sim_quit() {
	log_println(LEVEL_DEBUG, TAG, "sim_quit()");
	board_poweroff();
	m68k_end_timeslice();
	shouldexit = true;

}

void sim_reset() {

	log_println(LEVEL_DEBUG, TAG, "sim_reset()");

	m68k_pulse_reset();
}

bool sim_has_quit() {
	return shouldexit;
}
