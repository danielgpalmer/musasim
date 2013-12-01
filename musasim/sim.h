/**
 * \file sim.h
 */

#ifndef SIM__HEADER
#define SIM__HEADER

#include <stdbool.h>

void sim_setoptions(bool usebasicvideo, bool usebasicsound, bool osd);
bool sim_init(void);
void sim_quit(void);
void sim_reset(void);
void sim_tick(void);
void sim_togglepause(void);
void sim_sandboxviolated(void);
bool sim_has_quit(void);

// callbacks for musashi
void cpu_pulse_reset(void);
void cpu_pulse_stop(void);
void cpu_set_fc(unsigned int fc);
//

#define SIM_MAINCLOCK			64000000 // 64mhz#define SIM_CPUCLOCK_DIVIDER	4 // Divide the base clock by this to get the cpu clock#define SIM_CPUCLOCK 			(SIM_MAINCLOCK/SIM_CPUCLOCK_DIVIDER)
#define SIM_ONENANOSECOND 1000000000
#define SIM_CLOCKDURATION (SIM_ONENANOSECOND/SIM_MAINCLOCK)
#define SIM_CPUCLOCKDURATION (SIM_CLOCKDURATION * SIM_CPUCLOCK_DIVIDER)

// slots for the cards
#define SLOT_ROMCARD	0
#define SLOT_VIDEOCARD	1
#define SLOT_UARTCARD	2
#define SLOT_SOUNDCARD	3
#define SLOT_CFCARD		4
#define SLOT_DMACARD	5
#define SLOT_TIMERCARD	6
#define SLOT_INPUTCARD	7

void utils_printregisters(void);

#endif /* SIM__HEADER */
