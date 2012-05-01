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

static uint8_t* ram = (uint8_t*) 0x100000;

int _getchar(int timeout) {
	return -1;
}
void _sleep(unsigned long seconds) {
}
void _putchar(int c) {
	uart_putch((char) c);
}

int main(void) {

	printf("ymodem serial bootloader\n");
	printf("send your binary via ymodem now\n");
	ymodem_receive(ram, 0x100000);
	while(1){
	}
	return 0;
}
