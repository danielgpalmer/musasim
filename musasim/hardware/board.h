#define NUM_SLOTS 8

#include "cards/card.h"

void board_add_device(uint8_t slot, card *card);

unsigned int cpu_read_byte(unsigned int address);
unsigned int cpu_read_word(unsigned int address);
unsigned int cpu_read_long(unsigned int address);
void cpu_write_byte(unsigned int address, unsigned int value);
void cpu_write_word(unsigned int address, unsigned int value);
void cpu_write_long(unsigned int address, unsigned int value);

#define SLOT_ADDRESS_MASK 0x1FFFFF
#define SLOT_OFFSET(SLOTNUM) (SLOTNUM << 21)

/* Magic locations */
#define SIZE_MAGIC 0x100000
#define OFFSET_MAGIC 0x200000
#define MAX_MAGIC (OFFSET_MAGIC + (SIZE_MAGIC - 1))

unsigned int g_fc; /* Current function code from CPU */

