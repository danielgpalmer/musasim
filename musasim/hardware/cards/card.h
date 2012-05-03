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
	char* boardinfo;
	void (*init)();
	void (*dispose)();
	void (*reset)();
	void (*tick)();
	void (*irqack)();
	void (*busreqack)();
	bool (*validaddress)(uint32_t address);
	uint8_t (*read_byte)(uint32_t address);
	uint16_t (*read_word)(uint32_t address);
	uint32_t (*read_long)(uint32_t address);
	void (*write_byte)(uint32_t address, uint8_t value);
	void (*write_word)(uint32_t address, uint16_t value);
	void (*write_long)(uint32_t address, uint32_t value);
} card;

#endif /* CARD_H_ */
