/*
 * uartcard.c
 *
 *  Created on: Nov 26, 2011
 *      Author: daniel
 */

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <fcntl.h>
#include "uartcard.h"
#include "unistd.h"

#define FIFOSIZE 16
#define NUMOFCHANNELS 2

typedef struct {
	uint8_t receiver_buffer;
	uint8_t transmitter_holding;
	uint8_t interrupt_enable;
	uint8_t interrupt_identification;
	uint8_t fifo_control;
	uint8_t line_control;
	uint8_t modem_control;
	uint8_t line_status;
	uint8_t modem_status;
	uint8_t scratch;
	uint8_t divisor_latch_lsb;
	uint8_t divisor_latch_msb;
} registers;

typedef struct {
	registers registers;
	uint8_t rxfifo[FIFOSIZE];
	uint8_t txfifo[FIFOSIZE];
	int pty;
} channel;

channel channels[NUMOFCHANNELS];

void uart_init() {

	int pty;

	for (int i = 0; i < NUMOFCHANNELS; i++) {
		printf("Opening PTY for channel %d\n", i);
		pty = posix_openpt(O_RDWR | O_NOCTTY);
		if (pty == -1) {
			printf("FAILED!\n");
		}
		else {
			printf("Channel %d pty is %s\n", i, ptsname(pty));
			channels[i].pty = pty;
		}
	}
}

void uart_dispose() {

	for (int i = 0; i < NUMOFCHANNELS; i++) {
		close(channels[i].pty);
	}

}

/*
 * A3		->	Channel
 * A2 - A0	->	A2 - A0
 *
 */

#define LINECONTROL_DLAB 0x80

bool uart_dlab_high(registers* regs) {
	return (regs->line_control & LINECONTROL_DLAB) == LINECONTROL_DLAB ? true : false;
}

#define CHANNELMASK 0x08
#define REGISTERMASK 0x07

uint8_t* uart_decode_register(uint32_t address, bool write) {
	registers* regs = &(channels[(address & CHANNELMASK) >> 4].registers);
	uint8_t reg = (address & REGISTERMASK);

	switch (reg) {
		case 0x00:
			if (uart_dlab_high(regs)) {
				return &(regs->divisor_latch_lsb);
			}
			else {
				if (write) {
					return &(regs->transmitter_holding);
				}
				else {
					return &(regs->receiver_buffer);
				}
			}
		case 0x01:
			if (uart_dlab_high(regs)) {
				return &(regs->divisor_latch_msb);
			}
			else {
				return &(regs->interrupt_enable);
			}
		case 0x02:
			if (write) {
				return &(regs->fifo_control);
			}
			else {
				return &(regs->interrupt_identification);
			}
		case 0x03:
			return &(regs->line_control);
		case 0x04:
			return &(regs->modem_control);
		case 0x05:
			return &(regs->line_status);
		case 0x06:
			return &(regs->modem_status);
		case 0x07:
			return &(regs->scratch);
		default:
			return NULL;
	}

}

uint8_t uart_read_byte(uint32_t address) {
	uart_decode_register(address, false);
	return 0;
}

void uart_write_byte(uint32_t address, uint8_t value) {
	uart_decode_register(address, true);
}

card uartcard = {"UART CARD", uart_init, uart_dispose, NULL-, uart_read_byte, NULL, NULL, uart_write_byte, NULL, NULL};
