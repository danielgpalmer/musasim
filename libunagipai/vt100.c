/*
 * vt100.c
 *
 *  Created on: 19 May 2012
 *      Author: daniel
 */

#include "include/vt100.h"
#include <stdio.h>

#define ESC "\x1b"

void vt100_setattributes(){
	fputs(ESC"[31;43m",0);
}

void vt100_erase_screen() {
	fputs(ESC"[1J", 0);
}
