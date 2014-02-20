#define _POSIX_C_SOURCE 1993309

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <stdbool.h>
#include <stdint.h>
#include <unistd.h>
#include <glib.h>

#include "utils.h"
#include "sim.h"
#include "logging.h"
#include "throttler.h"
#include "ui/osd.h"
#include "ui/renderer.h"
#include "ui/input.h"
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

#define MINIMUMCPUCYCLESPERTICK 16

// state
static bool shouldexit = false;
static bool paused = false;
static bool initialised = false;

//
static bool osdonstate = false;

// machine configuration
static bool basicvideo = false;
static bool basicsound = false;

static const char TAG[] = "sim";

static long cycles = 0;

static void sim_updatesdl() {
	osd_update(throttler_speed(), throttler_overhead());
	input_update();
}

void cpu_pulse_reset(void) {
	log_println(LEVEL_INFO, TAG, "reset called");
	board_reset();
}

void cpu_pulse_stop(void) {
	uint16_t sr = (uint16_t) m68k_get_reg(NULL, M68K_REG_SR);
	uint32_t ssp = m68k_get_reg(NULL, M68K_REG_ISP);

	log_println(LEVEL_INFO, TAG, "CPU stopped SR:[0x%04x] SSP:[0x%08x]", sr,
			ssp);

	switch (sr) {
	case 0x0003: {
		uint32_t returnadd = board_read_long_cpu(ssp + 2); // FIXME should not use the cpu read function
		log_println(LEVEL_INFO, TAG,
				"Stop came from Illegal instruction at [0x%08x]", returnadd);
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

void sim_setoptions(bool usebasicvideo, bool usebasicsound,
bool osd) {
	if (initialised) {
		log_println(LEVEL_WARNING, TAG,
				"setoptions can only be called before the sim starts running");
		return;
	}

	basicvideo = usebasicvideo;
	basicsound = usebasicsound;
	osdonstate = osd;
}

bool sim_init() {
	if (initialised) {
		log_println(LEVEL_WARNING, TAG, "sim is apparently already running");
		return false;
	}

	log_println(LEVEL_DEBUG, TAG, "sim_init()");
	renderer_init();
	input_init();

	// todo this should be made static if possible as it stops
	// gcc from optimising out a lot of stuff
	board_add_device(SLOT_ROMCARD, &romcard);
	board_add_device(SLOT_VIDEOCARD, basicvideo ? &basicvideocard : &videocard);
	board_add_device(SLOT_UARTCARD, &uartcard);
	board_add_device(SLOT_SOUNDCARD, basicsound ? &basicsoundcard : &soundcard);
	board_add_device(SLOT_CFCARD, &compactflashinterfacecard);
	board_add_device(SLOT_DMACARD, &dmacard);
	board_add_device(SLOT_TIMERCARD, &timercard);
	board_add_device(SLOT_INPUTCARD, &inputcard);

	board_poweron();

	if (osd_init()) {
		if (osdonstate)
			osd_visible(true);
	} else
		return false;

	initialised = true;
	return true;
}

/*
 * Cranks the cpu and then cranks everything else by the same amount of cycles that the
 * cpu rolled over.
 */
void sim_tick() __attribute__((hot));
void sim_tick() {

	if (shouldexit) {
		return;
	}

	if (paused) {
		usleep(5000);
		sim_updatesdl();
	} else {
		throttler_starttick();
		if (cycles > SIM_MAINCLOCK / 30) {
			sim_updatesdl();
			if (shouldexit)
				return;
			cycles = 0;
		}

		// clamping the cycles to at least 16 avoids us getting stuck in a situation where there
		// aren't enough cycles being run to actually progress
		int cyclestoexecute = MAX(
				board_maxcycles(board_bestcasecycles()) / SIM_CPUCLOCK_DIVIDER,
				MINIMUMCPUCYCLESPERTICK);
		//log_println(LEVEL_INFO, TAG, "going to execute %d cpu cycles", cyclestoexecute);

		int cpucyclesexecuted;

		throttler_startcputick();
		if (!board_bus_locked()) {
			cpucyclesexecuted = m68k_execute(cyclestoexecute);
			//log_println(LEVEL_INFO, TAG, "executed %d cpu cycles", cpucyclesexecuted);
			if (shouldexit) {
				log_println(LEVEL_ALL, TAG, "oh noes!");
				sim_quit();
				return;
			}
		} else
			cpucyclesexecuted = cyclestoexecute;
		throttler_endcputick();

		cycles += cpucyclesexecuted;

		board_tick(cpucyclesexecuted * SIM_CPUCLOCK_DIVIDER,
				throttler_behind());
		renderer_render();

		throttler_endtick(cpucyclesexecuted);
	}

}

void sim_sandboxviolated() {
	log_println(LEVEL_INFO, TAG, "sim_sandboxviolated()");
	shouldexit = true;
	m68k_end_timeslice();
}

void sim_quit() {
	log_println(LEVEL_DEBUG, TAG, "sim_quit()");
	shouldexit = true;
	m68k_end_timeslice();
	board_poweroff();
	osd_dispose();
	log_shutdown();
}

void sim_reset() {
	log_println(LEVEL_DEBUG, TAG, "sim_reset()");
	m68k_pulse_reset();
}

void sim_togglepause() {
	paused = !paused;
	if (paused) {
		log_println(LEVEL_INFO, TAG, "sim is now paused");
	} else {
		log_println(LEVEL_INFO, TAG, "sim is now running");
	}
	board_pause(paused);
}

bool sim_has_quit() {
	return shouldexit;
}

void utils_printregisters() {
	printf(
			"D0[0x%08x]\nD1[0x%08x]\nD2[0x%08x]\nD3[0x%08x]\nD4[0x%08x]\nD5[0x%08x]\nD6[0x%08x]\nD7[0x%08x]\n", //
			m68k_get_reg(NULL, M68K_REG_D0), //
			m68k_get_reg(NULL, M68K_REG_D1), //
			m68k_get_reg(NULL, M68K_REG_D2), //
			m68k_get_reg(NULL, M68K_REG_D3), //
			m68k_get_reg(NULL, M68K_REG_D4), //
			m68k_get_reg(NULL, M68K_REG_D5), //
			m68k_get_reg(NULL, M68K_REG_D6), //
			m68k_get_reg(NULL, M68K_REG_D7));

	printf(
			"A0[0x%08x]\nA1[0x%08x]\nA2[0x%08x]\nA3[0x%08x]\nA4[0x%08x]\nA5[0x%08x]\nA6[0x%08x]\nA7[0x%08x]\n", //
			m68k_get_reg(NULL, M68K_REG_A0), //
			m68k_get_reg(NULL, M68K_REG_A1), //
			m68k_get_reg(NULL, M68K_REG_A2), //
			m68k_get_reg(NULL, M68K_REG_A3), //
			m68k_get_reg(NULL, M68K_REG_A4), //
			m68k_get_reg(NULL, M68K_REG_A5), //
			m68k_get_reg(NULL, M68K_REG_A6), //
			m68k_get_reg(NULL, M68K_REG_A7));

	printf("PC[0x%08x]\nPPC[0x%08x]\n", m68k_get_reg(NULL, M68K_REG_PC),
			m68k_get_reg(NULL, M68K_REG_PPC));
}
