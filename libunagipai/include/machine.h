/*
 * machine.h
 *
 *  Created on: Jan 19, 2012
 *      Author: daniel
 */

#ifndef MACHINE_H_
#define MACHINE_H_

#include <stdint.h>

uint16_t machine_getstatusregister();
void machine_setstatusregister(uint16_t value);

#endif /* MACHINE_H_ */
