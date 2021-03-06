/*
 * romcard.h
 *
 *  Created on: Nov 26, 2011
 *      Author: daniel
 */

#ifndef ROMCARD_H_
#define ROMCARD_H_

#include <stdbool.h>
#include "card.h"

bool romcard_loadrom(const char* path, bool elf);

const card romcard;

/* ROM and RAM sizes */
#define ROMCARD_OFFSET_ROM 0x0
#define OFFSET_RAM 0x100000
#define SIZE_ROM 0x100000
#define SIZE_RAM 0x100000
#define MAX_ROM (ROMCARD_OFFSET_ROM + (SIZE_ROM - 1))
#define MAX_RAM (OFFSET_RAM + (SIZE_RAM - 1))

#define ROMDISABLE_ADDRESS_0 (0xAAAAAA & 0xFFFFE)
#define ROMDISABLE_ADDRESS_1 (0x555555 & 0xFFFFE)
#define ROMDISABLE_DATA_0 0x55
#define ROMDISABLE_DATA_1 0xAA

#endif /* ROMCARD_H_ */
