#ifndef SIM__HEADER
#define SIM__HEADER

void sim_init();
void sim_quit();
void sim_reset();
void simstep();
void sim_tick();

void cpu_pulse_reset(void);
void cpu_set_fc(unsigned int fc);
int  cpu_irq_ack(int level);

#endif /* SIM__HEADER */
