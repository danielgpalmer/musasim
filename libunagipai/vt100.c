/*
 * vt100.c
 *
 *  Created on: 19 May 2012
 *      Author: daniel
 */

//http://www.termsys.demon.co.uk/vtansi.htm
#include "include/vt100.h"
#include <stdio.h>

#define ESC "\x1b"

void vt100_setattributes() {
	fputs(ESC"[31;43m", stdout);
}

void vt100_erase_screen() {
	fputs(ESC"[1J", stdout);
}

void vt100_cursor_home() {
	fputs(ESC"[H", stdout);
}
