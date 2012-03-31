/*
 * romcard.h
 *
 *  Created on: Nov 26, 2011
 *      Author: daniel
 */

#ifndef ROMCARD_H_
#define ROMCARD_H_

#include "card.h"
#include "stdbool.h"

bool romcard_loadrom(const char* path, bool elf);

const card romcard;

/* ROM and RAM sizes */
#define ROMCARD_OFFSET_ROM 0x0
#define OFFSET_RAM 0x100000
#define SIZE_ROM 0x100000
#define SIZE_RAM 0x100000
#define MAX_ROM (ROMCARD_OFFSET_ROM + (SIZE_ROM - 1))
#define MAX_RAM (OFFSET_RAM + (SIZE_RAM - 1))

#endif /* ROMCARD_H_ */
