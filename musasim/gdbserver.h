/*
 * gdbserver.h
 *
 *  Created on: Jan 11, 2012
 *      Author: daniel
 */

#ifndef GDBSERVER_H_
#define GDBSERVER_H_

#include <stdint.h>

void gdbserver_instruction_hook_callback();
void gdbserver_setport(int p);
void gdbserver_enteringinterrupt(int irq);
void gdb_hitstop(void);
void gdb_onpcmodified(uint32_t a);
void gdb_break(const char* reason);

uint8_t gdbserver_m68k_read_byte(uint32_t address);
uint16_t gdbserver_m68k_read_word(uint32_t address);
uint32_t gdbserver_m68k_read_long(uint32_t address);
void gdbserver_m68k_write_byte(uint32_t address, uint8_t value);
void gdbserver_m68k_write_word(uint32_t address, uint16_t value);
void gdbserver_m68k_write_long(uint32_t address, uint32_t value);

#endif /* GDBSERVER_H_ */
