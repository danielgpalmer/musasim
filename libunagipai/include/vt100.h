/*
 * vt100.h
 *
 *  Created on: 19 May 2012
 *      Author: daniel
 */

#ifndef VT100_H_
#define VT100_H_

#include <stdint.h>

void vt100_resetdevice(void);
void vt100_setattributes(void);
void vt100_erase_line(void);
void vt100_erase_screen(void);
void vt100_cursor_home(void);
void vt100_cursor_home_pos(uint8_t row, uint8_t col);
void vt100_cursor_save(void);
void vt100_cursor_unsave(void);
#endif /* VT100_H_ */
