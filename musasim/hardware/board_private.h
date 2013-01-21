/*
 * board_private.h
 *
 *  Created on: 21 Jan 2013
 *      Author: daniel
 */

#ifndef BOARD_PRIVATE_H_
#define BOARD_PRIVATE_H_

#include <stdbool.h>

unsigned int board_read_byte_internal(unsigned int address, bool skipchecks, const card* busmater);
unsigned int board_read_word_internal(unsigned int address, bool skipchecks, const card* busmaster);
unsigned int board_read_long_internal(unsigned int address, bool skipchecks, const card* busmaster);
void board_write_byte_internal(unsigned int address, unsigned int value, bool skipchecks, const card* busmaster);
void board_write_word_internal(unsigned int address, unsigned int value, bool skipchecks, const card* busmaster);
void board_write_long_internal(unsigned int address, unsigned int value, bool skipchecks, const card* busmaster);

#endif /* BOARD_PRIVATE_H_ */
