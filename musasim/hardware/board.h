#ifndef BOARD_H_
#define BOARD_H_

#define NUM_SLOTS 8

#include <stdbool.h>

#include "cards/card.h"

#define FCUSERDATA 0b001
#define FCUSERPROGRAM 0b010
#define FCSUPERVISORDATA 0b101
#define FCSUPERVISORPROGRAM 0b110
#define FCINTACK 0b111

#define DEFAULTMEMORYTYPE (CARDMEMORYTYPE_WRITABLE | CARDMEMORYTYPE_READABLE)

/* plug a card into the board, must be called before powering on */
bool board_add_device(uint8_t slot, const card* card);
/* plug the board in to the juice */
void board_poweron(void);
/* crank all of the cards plugged into the board */
void board_tick(int cyclesexecuted, bool behind);
/* reset the board and all of the cards */
void board_reset(void);
/* tell the board and cards that the emulation is being paused */
void board_pause(bool paused);
/* power the board down, frees all of the cards */
void board_poweroff(void);

unsigned int board_read_byte_cpu(unsigned int address);
unsigned int board_read_byte_busmaster(unsigned int address,
		const card* busmaster);
unsigned int board_read_word_cpu(unsigned int address);
unsigned int board_read_word_busmaster(unsigned int address,
		const card* busmaster);
unsigned int board_read_long_cpu(unsigned int address);
unsigned int board_read_long_busmaster(unsigned int address,
		const card* busmaster);

void board_write_byte_cpu(unsigned int address, unsigned int value);
void board_write_byte_busmaster(unsigned int address, unsigned int value,
		const card* busmaster);
void board_write_word_cpu(unsigned int address, unsigned int value);
void board_write_word_busmaster(unsigned int address, unsigned int value,
		const card* busmaster);
void board_write_long_cpu(unsigned int address, unsigned int value);
void board_write_long_busmaster(unsigned int address, unsigned int value,
		const card* busmaster);

void board_raise_interrupt(const card* card);
void board_lower_interrupt(const card* card);
int board_ack_interrupt(int level);

void board_lock_bus(const card* card);
void board_unlock_bus(const card* card);
bool board_bus_locked();

const card* board_getcardinslot(int slot);

int board_bestcasecycles(void);

int board_maxcycles(int numberofcyclesplanned);

#define SLOT_ADDRESS_MASK 0x1FFFFF
#define SLOT_OFFSET(SLOTNUM) (SLOTNUM << 21)
void board_setfc(unsigned int fc);
#endif
