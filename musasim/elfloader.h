/**
 *	\file elfloader.h
 *	\brief wrapper around libelf to load Elf binaries into ROM
 */

#ifndef ELFLOADER_H_
#define ELFLOADER_H_

#include <stdint.h>
#include <stdbool.h>

/**
 * Attempt to load an Elf into ROM
 */

bool elf_load(const char* path, uint8_t* dest, uint32_t destaddr, int maxlen);

#endif /* ELFLOADER_H_ */
