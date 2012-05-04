/*
 * card.h
 *
 *  Created on: Nov 26, 2011
 *      Author: daniel
 */

#ifndef CARD_H_
#define CARD_H_

#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>

typedef struct {
	char* boardinfo; // A simple description that can be displayed to aid debugging
	void (*init)(); // Do anything you need to do at program startup here
	void (*dispose)(); // Do anything you need to do before the program exits, i.e. free'ing stuff, here
	void (*reset)(); // This is called when the m68k pulls the reset line down
	void (*tick)(); //
	void (*irqack)();
	void (*busreqack)();
	bool (*validaddress)(uint32_t address); // This should return true if the address is valid within the card's address space
	uint8_t (*read_byte)(uint32_t address); // Read a byte from the cards address space
	uint16_t (*read_word)(uint32_t address); // ^ but for word (16bits)
	uint32_t (*read_long)(uint32_t address); // ^ but for long word (32bits)
	void (*write_byte)(uint32_t address, uint8_t value);
	void (*write_word)(uint32_t address, uint16_t value);
	void (*write_long)(uint32_t address, uint32_t value);
} card;

#endif /* CARD_H_ */
