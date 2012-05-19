/*
 * vt100.c
 *
 *  Created on: 19 May 2012
 *      Author: daniel
 */

//http://www.termsys.demon.co.uk/vtansi.htm
#include "include/vt100.h"
#include <inttypes.h>
#include <stdio.h>

#define ESC "\x1b"

static char buffer[24];

// terminal setup

void vt100_resetdevice() {
	fputs(ESC"[c", stdout);
}

void vt100_enablelinewrap() {
	fputs(ESC"[7h", stdout);
}

void vt100_disablelinewrap() {
	fputs(ESC"[7l", stdout);
}

//

// erasing text

void vt100_erase_screen() {
	fputs(ESC"[1J", stdout);
}

//

// cursor control

void vt100_cursor_home() {
	fputs(ESC"[H", stdout);
}

void vt100_cursor_home_pos(uint8_t row, uint8_t col) {
	snprintf(buffer, sizeof(buffer), ESC"[%"PRIu8";"PRIu8"H", row, col);
	fputs(buffer, stdout);
}

void vt100_cursor_up() {
	fputs(ESC"[A", stdout);
}

void vt100_cursor_upn(uint8_t count) {
	snprintf(buffer, sizeof(buffer), ESC"["PRIu8"A", count);
	fputs(buffer, stdout);
}

void vt100_cursor_down() {
	fputs(ESC"[B", stdout);
}

void vt100_cursor_downn(uint8_t count) {
	snprintf(buffer, sizeof(buffer), ESC"["PRIu8"B", count);
	fputs(buffer, stdout);
}
void vt100_cursor_forward() {
	fputs(ESC"[C", stdout);
}

void vt100_cursor_forwardn(uint8_t count) {
	snprintf(buffer, sizeof(buffer), ESC"["PRIu8"C", count);
	fputs(buffer, stdout);
}

void vt100_cursor_backward() {
	fputs(ESC"[D", stdout);
}

void vt100_cursor_backwardn(uint8_t count) {
	snprintf(buffer, sizeof(buffer), ESC"["PRIu8"D", count);
	fputs(buffer, stdout);
}
//

void vt100_setattributes() {
	fputs(ESC"[31;43m", stdout);
}
