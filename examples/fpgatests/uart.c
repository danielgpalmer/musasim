#include "uart_registers.h"
#include "uart_registermasks.h"
#include "uart.h"

char uart_getch() {
	while (!(uart_chan0_linestatus & LINESTATUS_DATAREADY)) {

	}

	return uart_chan0_rxtx;
}

void uart_putch(char ch) {

	while (!(uart_chan0_linestatus & LINESTATUS_TRANSMITTERHOLDINGREGISTEREMPTY)) {
		asm volatile ("nop \n\t");
	}
	uart_chan0_rxtx = ch;

}
