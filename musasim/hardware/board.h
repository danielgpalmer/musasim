#ifndef BOARD_H_
#define BOARD_H_

#define NUM_SLOTS 8

#include <stdbool.h>

#include "cards/card.h"

#define DEFAULTMEMORYTYPE (CARDMEMORYTYPE_WRITABLE | CARDMEMORYTYPE_READABLE)

void board_add_device(uint8_t slot, const card* card);
void board_tick(int cyclesexecuted);
void board_pause(bool paused);
void board_poweroff(void);

unsigned int board_read_byte_internal(unsigned int address, bool skipchecks, const card* busmater);
unsigned int board_read_byte_cpu(unsigned int address);
unsigned int board_read_byte_busmaster(unsigned int address, const card* busmaster);
unsigned int board_read_word_internal(unsigned int address, bool skipchecks, const card* busmaster);
unsigned int board_read_word_cpu(unsigned int address);
unsigned int board_read_word_busmaster(unsigned int address, const card* busmaster);
unsigned int board_read_long_internal(unsigned int address, bool skipchecks, const card* busmaster);
unsigned int board_read_long_cpu(unsigned int address);
unsigned int board_read_long_busmaster(unsigned int address, const card* busmaster);

void board_write_byte_internal(unsigned int address, unsigned int value, bool skipchecks, const card* busmaster);
void board_write_byte_cpu(unsigned int address, unsigned int value);
void board_write_byte_busmaster(unsigned int address, unsigned int value, const card* busmaster);
void board_write_word_internal(unsigned int address, unsigned int value, bool skipchecks, const card* busmaster);
void board_write_word_cpu(unsigned int address, unsigned int value);
void board_write_word_busmaster(unsigned int address, unsigned int value, const card* busmaster);
void board_write_long_internal(unsigned int address, unsigned int value, bool skipchecks, const card* busmaster);
void board_write_long_cpu(unsigned int address, unsigned int value);
void board_write_long_busmaster(unsigned int address, unsigned int value, const card* busmaster);

void board_raise_interrupt(const card* card);
void board_lower_interrupt(const card* card);
int board_ack_interrupt(int level);

void board_lock_bus(const card* card);
void board_unlock_bus(const card* card);
bool board_bus_locked();

const card* board_getcardinslot(int slot);

void board_reset(void);

int board_bestcasecycles(void);

int board_maxcycles(int numberofcyclesplanned);

#define SLOT_ADDRESS_MASK 0x1FFFFF
#define SLOT_OFFSET(SLOTNUM) (SLOTNUM << 21)
void board_setfc(unsigned int fc);
#endif
