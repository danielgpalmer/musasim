/**
 * \file sim.h
 */

#ifndef SIM__HEADER
#define SIM__HEADER

#include <stdbool.h>

void sim_setoptions(bool usebasicvideo, bool usebasicsound);
void sim_init();
void sim_quit();
void sim_reset();
void sim_tick();
bool sim_has_quit();

void cpu_pulse_reset(void);
void cpu_pulse_stop(void);
void cpu_set_fc(unsigned int fc);

#define SIM_TARGET_FREQUENCY 10000000 // 10mhz
#define SIM_CLOCKS_PERTICK 80 // this needs to high enough that the time a tick takes is a good number of micro seconds. On the other hand, if its too high emulation accuracy will be bad
#define SIM_TICKS_PERSECOND (SIM_TARGET_FREQUENCY/SIM_CLOCKS_PERTICK) //
#define SIM_USECSPERTICK (1000000 / SIM_TICKS_PERSECOND)

#define SLOT_ROMCARD 0
#define SLOT_VIDEOCARD 1
#define SLOT_UARTCARD 2
#define SLOT_SOUNDCARD 3
#define SLOT_CFCARD 4
#define SLOT_DMACARD 5
#define SLOT_INPUTCARD 7

#endif /* SIM__HEADER */
