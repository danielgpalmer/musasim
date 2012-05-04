/*
 * main.h
 *
 *  Created on: Jan 19, 2012
 *      Author: daniel
 */

#ifndef MAIN_H_
#define MAIN_H_

void interrupthandler() __attribute__ (( interrupt ));
void sound_handler() __attribute__ ((interrupt));
void vblank_handler() __attribute__ (( interrupt));
void uart_handler() __attribute__ (( interrupt));

#endif /* MAIN_H_ */
