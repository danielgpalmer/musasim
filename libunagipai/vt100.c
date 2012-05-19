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

void vt100_erase_endofline() {
	fputs(ESC"[K", stdout);
}

void vt100_erase_startofline() {
	fputs(ESC"[1K", stdout);
}

void vt100_erase_line() {
	fputs(ESC"[2K", stdout);
}

void vt100_erase_down() {
	fputs(ESC"[J", stdout);
}

void vt100_erase_up() {
	fputs(ESC"[1J", stdout);
}

void vt100_erase_screen() {
	fputs(ESC"[2J", stdout);
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

void vt100_cursor_save() {
	fputs(ESC"[s", stdout);
}

void vt100_cursor_unsave() {
	fputs(ESC"[u", stdout);
}

void vt100_cursor_savewithattrs() {
	fputs(ESC"[7", stdout);
}

void vt100_cursor_restorewithattrs() {
	fputs(ESC"[8", stdout);
}
//

// scrolling

void vt100_scrolling_screen() {
	fputs(ESC"[r", stdout);
}

void vt100_scrolling_screen_fromrowtorow(uint8_t from, uint8_t to) {

}

void vt100_scrolling_down() {
	fputs(ESC"D", stdout);
}

void vt100_scrolling_up() {
	fputs(ESC"M", stdout);
}
//

// tab control
void vt100_tabs_set() {
	fputs(ESC"H", stdout);
}

void vt100_tabs_clear() {
	fputs(ESC"[g", stdout);
}

void vt100_tabs_clearall() {
	fputs(ESC"[3g", stdout);
}
//

void vt100_setattributes() {
	fputs(ESC"[31;43m", stdout);
}
