/* Read/write macros */
#define READ_BYTE(BASE, ADDR) (BASE)[ADDR]
#define READ_WORD(BASE, ADDR) (((BASE)[ADDR]<<8) |			\
							  (BASE)[(ADDR)+1])
#define READ_LONG(BASE, ADDR) (((BASE)[ADDR]<<24) |			\
							  ((BASE)[(ADDR)+1]<<16) |		\
							  ((BASE)[(ADDR)+2]<<8) |		\
							  (BASE)[(ADDR)+3])

#define WRITE_BYTE(BASE, ADDR, VAL) (BASE)[ADDR] = (VAL) & 0xff
#define WRITE_WORD(BASE, ADDR, VAL) (BASE)[ADDR] = ((VAL)>>8) & 0xff;		\
									(BASE)[(ADDR)+1] = (VAL)&0xff
#define WRITE_LONG(BASE, ADDR, VAL) (BASE)[ADDR] = ((VAL)>>24) & 0xff;		\
									(BASE)[(ADDR)+1] = ((VAL)>>16)&0xff;	\
									(BASE)[(ADDR)+2] = ((VAL)>>8)&0xff;		\
									(BASE)[(ADDR)+3] = (VAL)&0xff

#define NUM_SLOTS 8

#include "cards/card.h"

void board_add_device(int slot, card *card);

unsigned int cpu_read_byte(unsigned int address);
unsigned int cpu_read_word(unsigned int address);
unsigned int cpu_read_long(unsigned int address);
void cpu_write_byte(unsigned int address, unsigned int value);
void cpu_write_word(unsigned int address, unsigned int value);
void cpu_write_long(unsigned int address, unsigned int value);

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




unsigned char g_rom[SIZE_ROM];					/* ROM */
unsigned char g_ram[SIZE_RAM];					/* RAM */
unsigned int g_fc;								/* Current function code from CPU */


