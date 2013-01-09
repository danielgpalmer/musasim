#include <stdbool.h>
#include "uart_registers.h"
#include "uart_registermasks.h"
#include "uart.h"

char uart_getch(uint8_t channel) {
	switch (channel) {
		case 0:
			while (!(uart_chan0_linestatus & LINESTATUS_DATAREADY)) {
			}
			return uart_chan0_rxtx ;
		case 1:
			while (!(uart_chan1_linestatus & LINESTATUS_DATAREADY)) {
			}
			return uart_chan1_rxtx ;
		default:
			return 0;
	}
}

bool uart_getch_nonblock(uint8_t channel, char* ch) {
	switch (channel) {
		case 0:
			if (uart_chan0_linestatus & LINESTATUS_DATAREADY) {
				*ch = uart_chan0_rxtx;
				return true;
			}
			return false;
		case 1:
			if (uart_chan1_linestatus & LINESTATUS_DATAREADY) {
				*ch = uart_chan1_rxtx;
				return true;
			}
			return false;
		default:
			return false;
	}
}

void uart_putch(uint8_t channel, char ch) {
	switch (channel) {
		case 0:
			uart_chan0_modemcontrol |= 0x04;
			while ((uart_chan0_linestatus & LINESTATUS_TRANSMITTERHOLDINGREGISTEREMPTY)
					!= LINESTATUS_TRANSMITTERHOLDINGREGISTEREMPTY) {
				// nop
			}
			uart_chan0_rxtx = ch;
			uart_chan0_modemcontrol &= ~0x04;
			break;
		case 1:
			while ((uart_chan1_linestatus & LINESTATUS_TRANSMITTERHOLDINGREGISTEREMPTY)
					!= LINESTATUS_TRANSMITTERHOLDINGREGISTEREMPTY) {
				// nop
			}
			uart_chan1_rxtx = ch;
			break;
	}
}

void uart_configureinterrupts(uint8_t channel, bool datareceived, bool transmitterempty, bool linestatus,
		bool modemstatus) {
	uart_chan0_interruptenable = 0x01;
}

void uart_configurefifos(uint8_t channel, bool enablefifos) {
	if (channel == 0) {
		uart_chan0_fifocontrol = FIFOCONTROL_ENABLE;
		uart_chan0_fifocontrol = FIFOCONTROL_ENABLE | FIFOCONTROL_RCVRFIFORESET | FIFOCONTROL_XMITFIFORESET;
	}
	else if (channel == 1) {
		uart_chan1_fifocontrol = FIFOCONTROL_ENABLE;
		uart_chan1_fifocontrol = FIFOCONTROL_ENABLE | FIFOCONTROL_RCVRFIFORESET | FIFOCONTROL_XMITFIFORESET;
	}
}

