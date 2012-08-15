#ifndef BOARD_H_
#define BOARD_H_

#define NUM_SLOTS 8

#include <stdbool.h>

#include "cards/card.h"

#define DEFAULTMEMORYTYPE CARDMEMORYTYPE_WRITABLE

void board_add_device(uint8_t slot, const card* card);
void board_tick(int cyclesexecuted);
void board_poweroff(void);

unsigned int board_read_byte(unsigned int address);
unsigned int board_read_word(unsigned int address);
unsigned int board_read_long(unsigned int address);
void board_write_byte(unsigned int address, unsigned int value);
void board_write_word(unsigned int address, unsigned int value);
void board_write_long(unsigned int address, unsigned int value);

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
