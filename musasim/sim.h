#ifndef SIM__HEADER
#define SIM__HEADER

void sim_init();
void sim_quit();
void sim_reset();
void simstep();
void sim_tick();

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
