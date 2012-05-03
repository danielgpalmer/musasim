/**
 * \file machine.h
 * \brief Functions to make m68k specific things like enabling interrupts nice and easy
 */

/*  Created on: Jan 19, 2012
 *      Author: daniel
 */

#ifndef LIBUNAGIPAI_MACHINE_H_
#define LIBUNAGIPAI_MACHINE_H_

#include <stdint.h>

/**
 * get the status register
 */
uint16_t machine_getstatusregister(void);

/**
 * set the status register
 */
void machine_setstatusregister(uint16_t value);

void machine_disablerom();

void machine_reset() __attribute__ ((section (".data")));

#endif /* MACHINE_H_ */
