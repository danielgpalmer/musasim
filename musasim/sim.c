#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <stdbool.h>
#include <stdint.h>
#include <time.h>
#include <glib.h>
#include <unistd.h>
#include <SDL.h>

#include <sys/time.h>

#include "sim.h"
#include "logging.h"
#include "musashi/m68k.h"

#include "hardware/board.h"
#include "hardware/cards/romcard.h"
#include "hardware/cards/basicvideo.h"
#include "hardware/cards/videocard.h"
#include "hardware/cards/uartcard.h"
#include "hardware/cards/basicsound.h"
#include "hardware/cards/soundcard.h"
#include "hardware/cards/compactflashinterfacecard.h"
#include "hardware/cards/inputcard.h"
#include "hardware/cards/dmacard.h"
#include "hardware/cards/timercard.h"

#define SIM_KEY_RESET	SDLK_r
#define SIM_KEY_PAUSE	SDLK_p
#define SIM_KEY_NMI		SDLK_n
#define SIM_KEY_QUIT	SDLK_ESCAPE

static bool shouldexit = false;
static bool paused = false;
static bool initialised = false;

static bool basicvideo = false;
static bool basicsound = false;

static const char TAG[] = "sim";

void cpu_pulse_reset(void) {
	log_println(LEVEL_INFO, TAG, "reset called");
	board_reset();
}

void cpu_pulse_stop(void) {

	uint16_t sr = (uint16_t) m68k_get_reg(NULL, M68K_REG_SR);
	uint32_t ssp = m68k_get_reg(NULL, M68K_REG_ISP);

	log_println(LEVEL_INFO, TAG, "CPU stopped SR:[0x%04x] SSP:[0x%08x]", sr, ssp);

	switch (sr) {
		case 0x0003: {
			uint32_t returnadd = board_read_long(ssp + 2);
			log_println(LEVEL_INFO, TAG, "Stop came from Illegal instruction at [0x%08x]", returnadd);
			char buff[1024];
			m68k_disassemble(buff, returnadd, M68K_CPU_TYPE_68000);
			log_println(LEVEL_INFO, TAG, "dis -> %s", buff);
		}
			break;
	}

	shouldexit = true;
}

/* Called when the CPU changes the function code pins */
void cpu_set_fc(unsigned int fc) {
	g_fc = fc;
}

void sim_setoptions(bool usebasicvideo, bool usebasicsound) {
	if (!initialised) {
		basicvideo = usebasicvideo;
		basicsound = usebasicsound;

	}
}

void sim_init() {
	log_println(LEVEL_DEBUG, TAG, "sim_init()");
	SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO | SDL_INIT_NOPARACHUTE);
	SDL_WM_SetCaption("musasim", "musasim");

	board_add_device(SLOT_ROMCARD, &romcard);
	board_add_device(SLOT_VIDEOCARD, basicvideo ? &basicvideocard : &videocard);
	board_add_device(SLOT_UARTCARD, &uartcard);
	board_add_device(SLOT_SOUNDCARD, basicsound ? &basicsoundcard : &soundcard);
	board_add_device(SLOT_CFCARD, &compactflashinterfacecard);
	board_add_device(SLOT_DMACARD, &dmacard);
	board_add_device(SLOT_TIMERCARD, &timercard);
	board_add_device(SLOT_INPUTCARD, &inputcard);
	initialised = true;
}

/* Subtract the `struct timeval' values X and Y,
 storing the result in RESULT.
 Return 1 if the difference is negative, otherwise 0.  */

static int timeval_subtract(result, x, y)
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

	log_println(LEVEL_INSANE, TAG, "sim_tick()");

	static struct timeval start, end, diff;
	static long int lastoutput = 0;
	static long int average = 0;
	static long int sleep = 0;

	if (shouldexit) {
		return;
	}

	// Check some keys
	SDL_PumpEvents();
	static SDL_Event events[10];
	if (SDL_PeepEvents(events, 1, SDL_GETEVENT, SDL_QUITMASK)) {
		log_println(LEVEL_INFO, TAG, "Window was closed");
		sim_quit();
		return;
	}

	for (int i = 0; i < SDL_PeepEvents(events, 10, SDL_PEEKEVENT, SDL_KEYDOWNMASK | SDL_KEYUPMASK); i++) {
		if (events[i].type == SDL_KEYUP) {
			switch (events[i].key.keysym.sym) {
				case SIM_KEY_NMI:
					m68k_set_irq(7);
					break;
				case SIM_KEY_RESET:
					break;
				case SIM_KEY_PAUSE:
					paused = !paused;
					if (paused) {
						log_println(LEVEL_INFO, TAG, "sim is now paused");
					}
					else {
						log_println(LEVEL_INFO, TAG, "sim is now running");
					}
					// drain all the events.. better way to fix this?
					while (SDL_PollEvent(events))
						;

					return;
				case SIM_KEY_QUIT:
					sim_quit();
					return;
				default:
					break;
			}
			break;
		}
	}

	//

	if (paused) {
		usleep(5000);
		return;
	}

	gettimeofday(&start, NULL);

	if (!board_bus_locked()) {
		m68k_execute(SIM_CLOCKS_PERTICK);
		if (shouldexit) {
			return;
		}
	}

	board_tick();

	gettimeofday(&end, NULL);

	timeval_subtract(&diff, &end, &start);

	average = (average + diff.tv_usec) / 2;

	if (diff.tv_sec > 0) {
		log_println(LEVEL_DEBUG, TAG, "tick took longer than one second");
	}

	if (lastoutput == 0) {
		lastoutput = end.tv_sec;
	}

	if (lastoutput < (end.tv_sec - 10)) {
		lastoutput = end.tv_sec;
		log_println(LEVEL_DEBUG, TAG, "tick is taking %ld us, %d target, %ld sleep", average, SIM_USECSPERTICK, sleep);
	}

	sleep = SIM_USECSPERTICK - average;
	if (sleep > 0) {
		usleep(sleep);
	}

}

void sim_quit() {
	log_println(LEVEL_DEBUG, TAG, "sim_quit()");
	board_poweroff();
	m68k_end_timeslice();
	shouldexit = true;
	log_shutdown();
}

void sim_reset() {

	log_println(LEVEL_DEBUG, TAG, "sim_reset()");

	m68k_pulse_reset();
}

bool sim_has_quit() {
	return shouldexit;
}
