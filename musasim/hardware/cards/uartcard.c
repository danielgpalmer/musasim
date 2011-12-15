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

#include "../../logging.h"

#include "../board.h"

#define FIFOSIZE 16
#define NUMOFCHANNELS 2

const char TAG[] = "uart";

typedef struct {
	uint8_t rxfifo[FIFOSIZE];
	uint8_t txfifo[FIFOSIZE];
	uint8_t txshift;
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
	unsigned int txclock;
} channel;

channel channels[NUMOFCHANNELS];

void uart_clearbit(uint8_t mask, uint8_t* target) {
	*target &= !mask;
}

void uart_setbit(uint8_t mask, uint8_t* target) {
	*target |= mask;
}

bool uart_bitset(uint8_t mask, uint8_t target) {
	return (target & mask) == mask ? true : false;
}

void uart_reset_channel(channel* chan) {
	chan->txclock = 0;
	chan->registers.line_status = 0x60;
	chan->registers.interrupt_identification = 0x01;
}

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
			printf("Channel %d pts is %s\n", i, ptsname(ptm));
			channels[i].ptm = ptm;
			int flags = fcntl(ptm, F_GETFL);
			if (!(flags & O_NONBLOCK)) {
				fcntl(ptm, F_SETFL, flags | O_NONBLOCK);
			}
		}

		uart_reset_channel(&(channels[i]));
	}
}

void uart_dispose() {

	for (int i = 0; i < NUMOFCHANNELS; i++) {
		close(channels[i].ptm);
	}

}

#define FIFOCONTROL_ENABLE 0x01
#define FIFOCONTROL_RCVRFIFORESET 0x02
#define FIFOCONTROL_XMITFIFORESET 0x04
#define FIFOCONTROL_DMAMODESELECT 0x08
#define FIFOCONTROL_RCVRTRIGGER 0xC0 // two bits
/*
 * A3		->	Channel
 * A2 - A0	->	A2 - A0
 *
 */

#define LINECONTROL_DLAB 0x80
#define LINECONTROL_SETBREAK 0x40
#define LINECONTROL_STICKPARITY 0x20
#define LINECONTROL_EVENPARITYSELECT 0x10
#define LINECONTROL_PARITYENABLE 0x08
#define LINECONTROL_NUMBEROFSTOPBITS 0x04
#define LINECONTROL_WORDLENGTHSELECT 0x03 // two bits

#define LINESTATUS_DATAREADY 0x01
#define LINESTATUS_OVERRUNERROR 0x02
#define LINESTATUS_PARITYERROR 0x04
#define LINESTATUS_FRAMINGERROR 0x08
#define LINESTATUS_BREAKINTERRUPT 0x20
#define LINESTATUS_TRANSMITTERHOLDINGREGISTEREMPTY 0x20
#define LINESTATUS_TRANSMITTEREMPTY 0x40
#define LINESTATUS_ERRORINRCVRFIFO 0x80

#define CHANNELMASK 0x08
#define REGISTERMASK 0x07

uint8_t* uart_decode_register(uint32_t address, bool write) {
	registers* regs = &(channels[(address & CHANNELMASK) >> 4].registers);
	uint8_t reg = (address & REGISTERMASK);

	switch (reg) {
		case 0x00:
			if (uart_bitset(LINECONTROL_DLAB, regs->line_control)) {
				return &(regs->divisor_latch_lsb);
			}
			else {
				if (write) {
					if (uart_bitset(FIFOCONTROL_ENABLE, regs->fifo_control)) {
						uint8_t* txslot = &(regs->txfifo[regs->txtop]);
						if (regs->txtop + 1 < FIFOSIZE) {
							regs->txtop += 1;
						}
						// Not right
						//else {
						//	uart_linestatus_cleartransmitterempty(regs);
						//}
						return txslot;
					}
					else {
						uart_clearbit(LINESTATUS_TRANSMITTEREMPTY, &(regs->line_status));
						return &(regs->txfifo[0]);
					}
				}
				else {
					if (uart_bitset(FIFOCONTROL_ENABLE, regs->fifo_control)) {
						uint8_t* rxslot = &(regs->rxfifo[regs->rxtop]);
						if (regs->rxtop - 1 > 0) {
							regs->rxtop -= 1;
						}
						else {
							uart_clearbit(LINESTATUS_DATAREADY, &(regs->line_status));
						}
						return rxslot;
					}
					else {
						uart_clearbit(LINESTATUS_DATAREADY, &(regs->line_status));
						return &(regs->rxfifo[0]);
					}
				}
			}
		case 0x01:
			if (uart_bitset(LINECONTROL_DLAB, regs->line_control)) {
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

	// slow this down a bit for now
	static int divider = 0;
	divider++;
	if (divider < 2048) {
		return;
	}

	divider = 0;

	//static int sixteendivider = 0;
	//static uint8_t byte;

	//sixteendivider++;
	//if (sixteendivider == 16) {
	//	sixteendivider = 0;
	//	for (int i = 0; i < NUMOFCHANNELS; i++) {
	//		//write(channels[i].ptm, "x", 1);
	//		int bytes = 0;
	//		if ((bytes = read(channels[i].ptm, &byte, 1)) != EAGAIN) {
	///			if (bytes > 0) {
	//				//printf("%c\n", byte);
	////			}
	//		}
	//	}
	//board_raise_interrupt(&uartcard);

	//}

	// Check the transmitter, if there is a byte start "transmitting it"

	//for (int i = 0; i < NUMOFCHANNELS; i++) {

	//for (int i = 0; i < NUMOFCHANNELS; i++) {
	for (int i = 0; i < 1; i++) {

		channel* channel = &(channels[i]);

		// Are we transmitting?
		if (!uart_bitset(LINESTATUS_TRANSMITTEREMPTY, channel->registers.line_status)) {

			log_println(LEVEL_DEBUG, TAG, "Transmitting .. clock %d", channel->txclock);

			channel->txclock++;
			// end of byte
			if (channel->txclock == 16) {
				channel->txclock = 0;

				log_println(LEVEL_DEBUG, TAG, "Transmitter has finished");

				// transmitter is now empty
				uart_setbit(LINESTATUS_TRANSMITTEREMPTY, &(channel->registers.line_status));
			}
		}

		// Are we not transmitting .. this check has to happen hence not an else if
		if (uart_bitset(LINESTATUS_TRANSMITTEREMPTY, (channels[i].registers.line_status))) {
			log_println(LEVEL_DEBUG, TAG, "Transmitter is empty!");

			// Check if we have data that is ready to be shifted out
			if (!uart_bitset(LINESTATUS_TRANSMITTERHOLDINGREGISTEREMPTY, channel->registers.line_status)) {

				log_println(LEVEL_DEBUG, TAG, "Holding register has something");

				// move data to transmitter .. which is really a cheat.. we just write it to the pty
				write(channel->ptm, &(channel->registers.txfifo[0]), 1);
				log_println(LEVEL_DEBUG, TAG, "Sent [%c]", channel->registers.txfifo[0]);

				// We're transmitting
				uart_clearbit(LINESTATUS_TRANSMITTEREMPTY, &(channel->registers.line_status));
			}
		}

	}

}

void uart_irq_ack() {
	//printf("uart_irq_ack()\n");
	board_lower_interrupt(&uartcard);
}

card uartcard = { "UART CARD", uart_init, uart_dispose, uart_tick, uart_irq_ack, NULL, uart_read_byte, NULL, NULL,
		uart_write_byte, NULL, NULL };
