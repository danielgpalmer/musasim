#define _POSIX_C_SOURCE 1993309

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <stdbool.h>
#include <stdint.h>
#include <time.h>
#include <math.h>
#include <glib.h>
#include <unistd.h>
#include <SDL.h>

#include <sys/time.h>

#include "utils.h"
#include "sim.h"
#include "osd.h"
#include "logging.h"
#include "musashi/m68k.h"

// all of the hardware headers
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

// keys that the sim uses
#define SIM_KEY_PAUSE			SDLK_F1
#define SIM_KEY_RESET			SDLK_F2
#define SIM_KEY_NMI				SDLK_F3
#define SIM_KEY_MUTE			SDLK_F4
#define SIM_KEY_TOGGLEOSD		SDLK_F5
#define SIM_KEY_TOGGLETHROTTLE	SDLK_F6
#define SIM_KEY_QUIT			SDLK_ESCAPE

// state
static bool throttle = true;
static bool shouldexit = false;
static bool paused = false;
static bool initialised = false;

// machine configuration
static bool basicvideo = false;
static bool basicsound = false;

static const char WINDOWTITLE[] = "musasim";
static const char TAG[] = "sim";

static void sim_updatesdl() {

	static int ticks = 0;
	if (ticks < SIM_TICKS_PERSECOND / 60) {
		ticks++;
		return;
	}
	ticks = 0;

	osd_update();

	// Check some keys
	SDL_PumpEvents();
	static SDL_Event events[10];
	if (SDL_PeepEvents(events, 1, SDL_GETEVENT, SDL_QUITMASK)) {
		log_println(LEVEL_INFO, TAG, "Window was closed");
		sim_quit();
		return;
	}

	// pick out the events that the sim wants
	for (int i = 0; i < SDL_PeepEvents(events, 10, SDL_PEEKEVENT, SDL_KEYDOWNMASK | SDL_KEYUPMASK); i++) {
		if (events[i].type == SDL_KEYUP) {
			switch (events[i].key.keysym.sym) {
				case SIM_KEY_NMI:
					m68k_set_irq(7);
					break;
				case SIM_KEY_RESET:
					break;
				case SIM_KEY_MUTE:
					break;
				case SIM_KEY_TOGGLETHROTTLE:
					throttle = !throttle;
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
	//videocard_refresh();
}

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
	if (initialised) {
		log_println(LEVEL_WARNING, TAG, "setoptions can only be called before the sim starts running");
		return;
	}

	basicvideo = usebasicvideo;
	basicsound = usebasicsound;
}

void sim_init() {
	if (initialised) {
		log_println(LEVEL_WARNING, TAG, "sim is apparently already running");
		return;
	}

	log_println(LEVEL_DEBUG, TAG, "sim_init()");
	SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO | SDL_INIT_NOPARACHUTE);
	SDL_WM_SetCaption(WINDOWTITLE, WINDOWTITLE);

	board_add_device(SLOT_ROMCARD, &romcard);
	board_add_device(SLOT_VIDEOCARD, basicvideo ? &basicvideocard : &videocard);
	board_add_device(SLOT_UARTCARD, &uartcard);
	board_add_device(SLOT_SOUNDCARD, basicsound ? &basicsoundcard : &soundcard);
	board_add_device(SLOT_CFCARD, &compactflashinterfacecard);
	board_add_device(SLOT_DMACARD, &dmacard);
	board_add_device(SLOT_TIMERCARD, &timercard);
	board_add_device(SLOT_INPUTCARD, &inputcard);

	osd_init();

	initialised = true;
}

void sim_tick() {

	static long int owed = 0;
	struct timespec start, end, sleep;

	if (shouldexit) {
		return;
	}

	if (paused) {
		usleep(5000);
		return;
	}

	int cpucyclesexecuted = SIM_CPUCLOCKS_PERTICK;

	clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &start);
	if (!board_bus_locked()) {
		//TODO what causes the emulator to run less cycles than we want?
		cpucyclesexecuted = m68k_execute(SIM_CPUCLOCKS_PERTICK);
	}

	board_tick(cpucyclesexecuted * SIM_CPUCLOCK_DIVIDER);
	sim_updatesdl();
	clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &end);

	long int timetaken = timespecdiff(&start, &end)->tv_nsec;
	if (timetaken > SIM_NANOSECSPERTICK * 2) {
		log_println(LEVEL_INFO, TAG, "timetaken got clamped, was %ld", timetaken);
		timetaken = SIM_NANOSECSPERTICK * 2;
	}
	long int overrun = timetaken - SIM_NANOSECSPERTICK;
	long int sleeptime = 0;

	owed += overrun;
	//log_println(LEVEL_INFO, TAG, "%ld overrun owed %ld", overrun, owed);
	if (owed < 0) { // If the amount of nanos owed has become negative we're in the black, so sleep for a bit
		sleeptime = labs(owed);
		//log_println(LEVEL_INFO, TAG, "sleeping for %ld", sleeptime);
		owed = 0;
	}

	//if (sleeptime > 5000) {
	//	sleeptime = 5000;
	//}

	if (throttle && owed == 0 && sleeptime > 0) {
		sleep.tv_nsec = sleeptime;
		nanosleep(&sleep, NULL);
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
