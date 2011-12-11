#ifndef SIM__HEADER
#define SIM__HEADER

#include <stdbool.h>

void sim_init();
void sim_quit();
void sim_reset();
void sim_tick();
bool sim_has_quit();

void cpu_pulse_reset(void);
void cpu_set_fc(unsigned int fc);

#define SLOT_ROMCARD 0
#define SLOT_VIDEOCARD 1
#define SLOT_UARTCARD 2
#define SLOT_SOUNDCARD 3
#define SLOT_CFCARD 4
#define SLOT_DMACARD 5
#define SLOT_INPUTCARD 7

#endif /* SIM__HEADER */
