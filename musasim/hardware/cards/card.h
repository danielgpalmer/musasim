/*
 * card.h
 *
 *  Created on: Nov 26, 2011
 *      Author: daniel
 */

#ifndef CARD_H_
#define CARD_H_

typedef struct {
	void (*read_byte)(uint32_t address);
	void (*read_word)(uint32_t address);
	void (*read_long)(uint32_t address);
	void (*write_byte)(uint32_t address);
	void (*write_word)(uint32_t address);
	void (*write_long)(uint32_t address);
} card;

#endif /* CARD_H_ */
