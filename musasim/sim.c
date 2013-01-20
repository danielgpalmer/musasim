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

//
static bool osdonstate = false;

// machine configuration
static bool basicvideo = false;
static bool basicsound = false;

static const char WINDOWTITLE[] = "musasim";
static const char TAG[] = "sim";

static void sim_updatesdl() {

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
				case SIM_KEY_TOGGLEOSD:
					log_println(LEVEL_INFO, TAG, "toggling osd");
					osd_toggle();
					break;
				case SIM_KEY_PAUSE:
					paused = !paused;
					if (paused) {
						log_println(LEVEL_INFO, TAG, "sim is now paused");
					}
					else {
						log_println(LEVEL_INFO, TAG, "sim is now running");
					}
					board_pause(paused);
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
			uint32_t returnadd = board_read_long_cpu(ssp + 2); // FIXME should not use the cpu read function
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
	static unsigned int currentfc = 0xFF; // the incoming fc will never be this value
	if (currentfc != fc) {
		board_setfc(fc);
		currentfc = fc;
	}
}

void sim_setoptions(bool usebasicvideo, bool usebasicsound, bool osd) {
	if (initialised) {
		log_println(LEVEL_WARNING, TAG, "setoptions can only be called before the sim starts running");
		return;
	}

	basicvideo = usebasicvideo;
	basicsound = usebasicsound;
	osdonstate = osd;
}

void sim_init() {
	if (initialised) {
		log_println(LEVEL_WARNING, TAG, "sim is apparently already running");
		return;
	}

	g_thread_init(NULL );

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

	board_poweron();

	osd_init();
	if (osdonstate)
		osd_visible(true);

	initialised = true;
}

/*
 * Cranks the cpu and then cranks everything else by the same amount of cycles that the
 * cpu rolled over.
 */

void sim_tick() {

	struct timespec start, end;
	static long int owed = 0;

	sim_updatesdl();

	if (shouldexit) {
		return;
	}

	if (paused)
		usleep(5000);
	else {

		int cyclestoexecute = board_maxcycles(board_bestcasecycles()) / SIM_CPUCLOCK_DIVIDER;
		// this avoids us getting stuck
		if (cyclestoexecute < 16)
			cyclestoexecute = 16;

		//log_println(LEVEL_INFO, TAG, "going to execute %d cpu cycles", cyclestoexecute);

		int cpucyclesexecuted;
		clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &start);
		if (!board_bus_locked()) {
			//TODO what causes the emulator to run less cycles than we want?
			cpucyclesexecuted = m68k_execute(cyclestoexecute);
			//log_println(LEVEL_INFO, TAG, "executed %d cpu cycles", cpucyclesexecuted);
			if (shouldexit) {
				log_println(LEVEL_ALL, TAG, "oh noes!");
				sim_quit();
				return;
			}
		}
		else
			cpucyclesexecuted = cyclestoexecute;

		board_tick(cpucyclesexecuted * SIM_CPUCLOCK_DIVIDER);
		clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &end);

		long int timetaken = timespecdiff(&start, &end)->tv_nsec;
		long int target = SIM_CPUCLOCKDURATION * cpucyclesexecuted;

		if (timetaken > 100000) {
			//log_println(LEVEL_INFO, TAG, "CLAMP!");
			timetaken = 100000;
		}

		long int diff = target - timetaken;
		if (diff > 0) {
			owed -= diff;
			if (owed < 0) {
				usleep(abs(owed) / (1000 + 100));
				owed = 0;
			}
		}
		else {
			owed += labs(diff);
		}
		//log_println(LEVEL_INFO, TAG, "target %ld, actual %ld, owed %ld", target, timetaken, owed);
	}

}

void sim_sandboxviolated() {
	log_println(LEVEL_INFO, TAG, "sim_sandboxviolated()");
	m68k_end_timeslice();
	shouldexit = true;
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

void utils_printregisters() {
	printf("D0[0x%08x]\nD1[0x%08x]\nD2[0x%08x]\nD3[0x%08x]\nD4[0x%08x]\nD5[0x%08x]\nD6[0x%08x]\nD7[0x%08x]\n", //
			m68k_get_reg(NULL, M68K_REG_D0), //
			m68k_get_reg(NULL, M68K_REG_D1), //
			m68k_get_reg(NULL, M68K_REG_D2), //
			m68k_get_reg(NULL, M68K_REG_D3), //
			m68k_get_reg(NULL, M68K_REG_D4), //
			m68k_get_reg(NULL, M68K_REG_D5), //
			m68k_get_reg(NULL, M68K_REG_D6), //
			m68k_get_reg(NULL, M68K_REG_D7));

	printf("A0[0x%08x]\nA1[0x%08x]\nA2[0x%08x]\nA3[0x%08x]\nA4[0x%08x]\nA5[0x%08x]\nA6[0x%08x]\nA7[0x%08x]\n", //
			m68k_get_reg(NULL, M68K_REG_A0), //
			m68k_get_reg(NULL, M68K_REG_A1), //
			m68k_get_reg(NULL, M68K_REG_A2), //
			m68k_get_reg(NULL, M68K_REG_A3), //
			m68k_get_reg(NULL, M68K_REG_A4), //
			m68k_get_reg(NULL, M68K_REG_A5), //
			m68k_get_reg(NULL, M68K_REG_A6), //
			m68k_get_reg(NULL, M68K_REG_A7));

	printf("PC[0x%08x]\nPPC[0x%08x]\n", m68k_get_reg(NULL, M68K_REG_PC), m68k_get_reg(NULL, M68K_REG_PPC));
}
