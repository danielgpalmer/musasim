/*
 * input.h
 *
 *  Created on: 6 May 2013
 *      Author: daniel
 */

#ifndef INPUT_H_
#define INPUT_H_

#include <stdbool.h>

void input_init(void);
void input_update(void);
bool input_pump_and_checkquit();

#endif /* INPUT_H_ */
