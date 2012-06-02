#include <stdbool.h>
#include "uart_registers.h"
#include "uart_registermasks.h"
#include "uart.h"

char uart_getch() {
	while (!(uart_chan0_linestatus & LINESTATUS_DATAREADY)) {

	}

	return uart_chan0_rxtx;
}

bool uart_getch_nonblock(char* ch) {
	if (uart_chan0_linestatus & LINESTATUS_DATAREADY) {
		*ch = uart_chan0_rxtx;
		return true;
	}
	return false;
}

void uart_putch(char ch) {
	uart_chan0_modemcontrol |= 0x04;
	while ((uart_chan0_linestatus & LINESTATUS_TRANSMITTERHOLDINGREGISTEREMPTY)
			!= LINESTATUS_TRANSMITTERHOLDINGREGISTEREMPTY) {
		// nop
	}
	uart_chan0_rxtx = ch;
	uart_chan0_modemcontrol &= ~0x04;
}

void uart_configureinterrupts(uint8_t channel, bool datareceived, bool transmitterempty, bool linestatus,
		bool modemstatus) {

}

char uart_getch1() {
	while (!(uart_chan1_linestatus & LINESTATUS_DATAREADY)) {

	}

	return uart_chan1_rxtx;
}

bool uart_getch_nonblock1(char* ch) {
	if (uart_chan1_linestatus & LINESTATUS_DATAREADY) {
		*ch = uart_chan1_rxtx;
		return true;
	}
	return false;
}

void uart_putch1(char ch) {

	while ((uart_chan1_linestatus & LINESTATUS_TRANSMITTERHOLDINGREGISTEREMPTY)
			!= LINESTATUS_TRANSMITTERHOLDINGREGISTEREMPTY) {
		// nop
	}
	uart_chan1_rxtx = ch;

}
