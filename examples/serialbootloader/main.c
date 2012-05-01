/*
 * main.c
 *
 *  Created on: Jul 17, 2011
 *      Author: daniel
 */

#include <stdio.h>
#include <stdint.h>
#include "ymodem.h"
#include "uart.h"
#include "uart_registers.h"
#include "uart_registermasks.h"

static uint8_t* ram = (uint8_t*) 0x100000;

int _getchar(int timeout) {
	char ch;
	return uart_getch_nonblock1(&ch) ? (int) ch : -1;
}
void _sleep(unsigned long seconds) {
}
void _putchar(int c) {
	uart_putch1((char) c);
}

int main(void) {

	printf("ymodem serial bootloader\n");
	printf("send your binary via ymodem now\n");
	//*uart_chan1_fifocontrol = FIFOCONTROL_ENABLE;
	ymodem_receive(ram, 0x100000);
	while(1){
	}
	return 0;
}
