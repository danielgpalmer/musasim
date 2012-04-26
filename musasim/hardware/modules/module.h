/*
 * module.h
 *
 *  Created on: Apr 25, 2012
 *      Author: daniel
 */

#ifndef MODULE_H_
#define MODULE_H_

typedef struct {
	void (*init)();
	void (*dispose)();
	void (*tick)();
	uint8_t (*read_byte)(uint16_t address);
	uint16_t (*read_word)(uint16_t address);
	uint32_t (*read_long)(uint16_t address);
	void (*write_byte)(uint16_t address, uint8_t value);
	void (*write_word)(uint16_t address, uint16_t value);
	void (*write_long)(uint16_t address, uint32_t value);
} module;

#endif /* MODULE_H_ */
