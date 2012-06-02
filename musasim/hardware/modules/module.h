/*
 * module.h
 *
 *  Created on: Apr 25, 2012
 *      Author: daniel
 */

#ifndef MODULE_H_
#define MODULE_H_

#include <stdint.h>

typedef struct {
	void* (*init)(void* context);
	void (*dispose)(void* context);
	void (*tick)(void* context);
	uint8_t (*read_byte)(void* context, uint16_t address);
	uint16_t (*read_word)(void* context, uint16_t address);
	uint32_t (*read_long)(void* context, uint16_t address);
	void (*write_byte)(void* context, uint16_t address, uint8_t value);
	void (*write_word)(void* context, uint16_t address, uint16_t value);
	void (*write_long)(void* context, uint16_t address, uint32_t value);
} module;

#endif /* MODULE_H_ */
