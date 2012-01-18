#include "include/uart.h"

#include "../musasim/genheader/uart.h"

void uart_readch() {

}

void uart_putch(char ch) {

	while ((*(uart_chan0_linestatus) & LINESTATUS_TRANSMITTERHOLDINGREGISTEREMPTY)
			!= LINESTATUS_TRANSMITTERHOLDINGREGISTEREMPTY) {
		// nop
	}
	*uart_chan0_rxtx = ch;

}
