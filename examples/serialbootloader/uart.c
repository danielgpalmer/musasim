#include <stdbool.h>

#include "uart_registers.h"
#include "uart_registermasks.h"
#include "uart.h"

char uart_getch() {
	while (!(*(uart_chan0_linestatus) & LINESTATUS_DATAREADY)) {

	}

	return *uart_chan0_rxtx;
}

bool uart_getch_nonblock(char* ch) {
        if(*(uart_chan0_linestatus) & LINESTATUS_DATAREADY) {
		*ch = *uart_chan0_rxtx;
        	return true;
	}
        return false;
}

void uart_putch(char ch) {

	while ((*(uart_chan0_linestatus) & LINESTATUS_TRANSMITTERHOLDINGREGISTEREMPTY)
			!= LINESTATUS_TRANSMITTERHOLDINGREGISTEREMPTY) {
		// nop
	}
	*uart_chan0_rxtx = ch;

}

char uart_getch1() {
        while (!(*(uart_chan1_linestatus) & LINESTATUS_DATAREADY)) {

        }

        return *uart_chan1_rxtx;
}

bool uart_getch_nonblock1(char* ch) {
        if(*(uart_chan1_linestatus) & LINESTATUS_DATAREADY) {
                *ch = *uart_chan1_rxtx;
                return true;
        }
        return false;
}

void uart_putch1(char ch) {

        while ((*(uart_chan1_linestatus) & LINESTATUS_TRANSMITTERHOLDINGREGISTEREMPTY)
                        != LINESTATUS_TRANSMITTERHOLDINGREGISTEREMPTY) {
                // nop
        }
        *uart_chan1_rxtx = ch;

}

