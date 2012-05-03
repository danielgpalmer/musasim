/*
 * main.c
 *
 *  Created on: Jul 17, 2011
 *      Author: daniel
 */

#include <stdio.h>
#include <stdint.h>
#include <errno.h>
#include "ymodem.h"
#include "uart.h"
#include "uart_registers.h"
#include "uart_registermasks.h"

static uint8_t* ram = (uint8_t*) 0x100000;

int _getchar(int timeout) {
	uint8_t ch;
	for (int i = 0; i < 256; i++) {
		//for(int j = 0; j < 16; j++){
		if (uart_getch_nonblock1(&ch)) {
			return ch;
		}
		//printf("spinning\n");
		//}
	}
	return -1;
}
void _sleep(unsigned long seconds) {
}
void _putchar(int c) {
	uart_putch1((char) c);
}

int main(void) {

	printf("ymodem serial bootloader\n");
	printf("send your binary via ymodem now\n");
	*uart_chan1_fifocontrol = FIFOCONTROL_ENABLE;
	*uart_chan1_fifocontrol = FIFOCONTROL_ENABLE | FIFOCONTROL_RCVRFIFORESET | FIFOCONTROL_XMITFIFORESET;
	machine_disablerom();
	machine_reset();
	while (1) {
		if (ymodem_receive(ram, 0xf8000)) {
			printf("Loaded ..\n");
			while (1) {

			}
		}
		else {
			printf("Failed - Error %d\n", errno);
		}
	}
	return 0;
}
