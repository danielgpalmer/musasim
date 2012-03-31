/*
 * elfloader.h
 *
 *  Created on: Mar 30, 2012
 *      Author: daniel
 */

#ifndef ELFLOADER_H_
#define ELFLOADER_H_

#include <stdint.h>
#include <stdbool.h>

bool elf_load(const char* path, uint8_t* dest, uint32_t destaddr, int maxlen);

#endif /* ELFLOADER_H_ */
