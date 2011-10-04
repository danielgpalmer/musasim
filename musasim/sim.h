#ifndef SIM__HEADER
#define SIM__HEADER

void sim_init();
void sim_quit();
void simreset();
void simstep();
void simtick();

unsigned int cpu_read_byte(unsigned int address);
unsigned int cpu_read_word(unsigned int address);
unsigned int cpu_read_long(unsigned int address);
void cpu_write_byte(unsigned int address, unsigned int value);
void cpu_write_word(unsigned int address, unsigned int value);
void cpu_write_long(unsigned int address, unsigned int value);
void cpu_pulse_reset(void);
void cpu_set_fc(unsigned int fc);
int  cpu_irq_ack(int level);

/* ROM and RAM sizes */
#define SIZE_ROM 0x100000
#define OFFSET_ROM 0x0
#define MAX_ROM (OFFSET_ROM + (SIZE_ROM - 1))
#define SIZE_RAM 0x100000
#define OFFSET_RAM 0x100000
#define MAX_RAM (OFFSET_RAM + (SIZE_RAM - 1))

/* Magic locations */
#define SIZE_MAGIC 0x100000
#define OFFSET_MAGIC 0x200000
#define MAX_MAGIC (OFFSET_MAGIC + (SIZE_MAGIC - 1))

/* Video */

#define SIZE_VIDEO 0x12C000
#define OFFSET_VIDEO (OFFSET_MAGIC + SIZE_MAGIC)
#define MAX_VIDEO (OFFSET_VIDEO + (SIZE_VIDEO - 1))


unsigned char g_rom[SIZE_ROM];					/* ROM */
unsigned char g_ram[SIZE_RAM];					/* RAM */
unsigned int g_fc;								/* Current function code from CPU */



#endif /* SIM__HEADER */
