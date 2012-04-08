#include "include/uart_registers.h"
#include "include/uart_registermasks.h"
#include "include/uart.h"

char uart_getch() {
	while (!(*(uart_chan0_linestatus) & LINESTATUS_DATAREADY)) {

	}

	return *uart_chan0_rxtx;
}

void uart_putch(char ch) {

	while ((*(uart_chan0_linestatus) & LINESTATUS_TRANSMITTERHOLDINGREGISTEREMPTY)
			!= LINESTATUS_TRANSMITTERHOLDINGREGISTEREMPTY) {
		// nop
	}
	*uart_chan0_rxtx = ch;

}
