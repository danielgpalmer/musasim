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
#include <unistd.h>
#include <errno.h>

#define FIFOSIZE 16
#define NUMOFCHANNELS 2

typedef struct {
	uint8_t rxfifo[FIFOSIZE];
	uint8_t txfifo[FIFOSIZE];
	uint8_t rxtop;
	uint8_t txtop;
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
	int ptm;
} channel;

channel channels[NUMOFCHANNELS];

void uart_init() {

	int ptm;

	for (int i = 0; i < NUMOFCHANNELS; i++) {
		printf("Opening PTY for channel %d\n", i);
		ptm = posix_openpt(O_RDWR | O_NOCTTY);
		if (ptm == -1) {
			printf("FAILED!\n");
		}
		else {
			grantpt(ptm);
			unlockpt(ptm);
			printf("Channel %d ptm is %s\n", i, ptsname(ptm));
			channels[i].ptm = ptm;
			int flags = fcntl(ptm, F_GETFL);
			if (!(flags & O_NONBLOCK)) {
				fcntl(ptm, F_SETFL, flags | O_NONBLOCK);
			}
		}
	}
}

void uart_dispose() {

	for (int i = 0; i < NUMOFCHANNELS; i++) {
		close(channels[i].ptm);
	}

}

#define FIFOCONTROL_ENABLE 0x80
#define FIFOCONTROL_RCVRFIFORESET 0x40
#define FIFOCONTROL_XMITFIFORESET 0x20
#define FIFOCONTROL_DMAMODESELECT 0x10
#define FIFOCONTROL_RCVRTRIGGER 0x03

bool uart_fifo_enabled(registers* regs) {
	return ((regs->fifo_control & FIFOCONTROL_ENABLE) == FIFOCONTROL_ENABLE) ? true : false;
}

uint8_t uart_fifo_rcvrtrigger(registers* regs) {
	return regs->fifo_control && FIFOCONTROL_RCVRTRIGGER;
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

#define LINESTATUS_DATAREADY 0x80
#define LINESTATUS_OVERRUNERROR 0x40
#define LINESTATUS_PARITYERROR 0x20
#define LINESTATUS_FRAMINGERROR 0x10
#define LINESTATUS_BREAKINTERRUPT 0x08
#define LINESTATUS_TRANSMITTERHOLDINGREGISTEREMPTY 0x04
#define LINESTATUS_TRANSMITTEREMPTY 0x02
#define LINESTATUS_ERRORINRCVRFIFO 0x01

void uart_linestatus_setdataready() {

}

void uart_linestatus_cleardataready() {

}

void uart_linestatus_settransmitterempty() {

}

void uart_linestatus_cleartransmitterempty() {

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
					if (uart_fifo_enabled(regs)) {
						uint8_t* txslot = &(regs->txfifo[regs->txtop]);
						if (regs->txtop + 1 < FIFOSIZE) {
							regs->txtop += 1;
						}
						else {
							uart_linestatus_cleartransmitterempty();
						}
						return txslot;
					}
					else {
						uart_linestatus_cleartransmitterempty();
						return &(regs->txfifo[0]);
					}
				}
				else {
					if (uart_fifo_enabled(regs)) {
						uint8_t* rxslot = &(regs->rxfifo[regs->rxtop]);
						if (regs->rxtop - 1 > 0) {
							regs->rxtop -= 1;
						}
						else {
							uart_linestatus_cleardataready();
						}
						return rxslot;
					}
					else {
						uart_linestatus_cleardataready();
						return &(regs->rxfifo[0]);
					}
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
	uint8_t* reg = uart_decode_register(address, false);
	return *reg;
}

void uart_write_byte(uint32_t address, uint8_t value) {
	uint8_t* reg = uart_decode_register(address, true);
	*reg = value;
}

void uart_tick() {

	static int sixteendivider = 0;
	static uint8_t byte;

	sixteendivider++;
	if (sixteendivider == 16) {
		sixteendivider = 0;
		for (int i = 0; i < NUMOFCHANNELS; i++) {
			//write(channels[i].ptm, "x", 1);
			int bytes = 0;
			if ((bytes = read(channels[i].ptm, &byte, 1)) != EAGAIN) {
				if(bytes > 0){
					printf("%c\n", byte);
				}
			}
		}
	}

}

card uartcard = { "UART CARD", uart_init, uart_dispose, uart_tick, uart_read_byte, NULL, NULL, uart_write_byte, NULL,
		NULL };
