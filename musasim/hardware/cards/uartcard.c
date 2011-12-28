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
#include <termios.h>
#include "uartcard.h"
#include "uartregistermasks.h"
#include <unistd.h>
#include <errno.h>

#include "../../logging.h"

#include "../board.h"

#define FIFOSIZE 16
#define NUMOFCHANNELS 2

static const char TAG[] = "uart";

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
	int clockdivider;
	bool dlabwilllatch;
	bool intpending_forth;
	bool intpending_third;
	bool intpending_second;
	bool intpending_highest;
} channel;

channel channels[NUMOFCHANNELS];

void uart_clearbit(uint8_t mask, uint8_t* target) {
	*target &= ~mask;
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
	chan->clockdivider = 0;
}

void uart_init() {

	int ptm;

	struct termios tattr;

	for (int i = 0; i < NUMOFCHANNELS; i++) {
		log_println(LEVEL_INFO, TAG, "Opening PTY for channel %d", i);
		ptm = posix_openpt(O_RDWR | O_NOCTTY);
		if (ptm == -1) {
			log_println(LEVEL_INFO, TAG, "failed opening PTY!");
		}
		else {
			grantpt(ptm);
			unlockpt(ptm);
			log_println(LEVEL_INFO, TAG, "Channel %d pts is %s", i, ptsname(ptm));
			channels[i].ptm = ptm;
			int flags = fcntl(ptm, F_GETFL);
			if (!(flags & O_NONBLOCK)) {
				fcntl(ptm, F_SETFL, flags | O_NONBLOCK);
			}
			tcgetattr(ptm, &tattr);
			cfmakeraw(&tattr);
			tcsetattr(ptm, 0, &tattr);
		}

		uart_reset_channel(&(channels[i]));
	}
}

void uart_dispose() {

	for (int i = 0; i < NUMOFCHANNELS; i++) {
		close(channels[i].ptm);
	}

}

/*
 * A3		->	Channel
 * A2 - A0	->	A2 - A0
 *
 */

#define CHANNELMASK 0x08
#define REGISTERMASK 0x07

uint8_t* uart_decode_register(uint32_t address, bool write) {
	channel* chan = &(channels[(address & CHANNELMASK) >> 4]);
	registers* regs = &(chan->registers);
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
						log_println(LEVEL_INSANE, TAG, "Host wrote a byte");
						if (!uart_bitset(LINESTATUS_TRANSMITTERHOLDINGREGISTEREMPTY, regs->line_status)) {
							log_println(LEVEL_DEBUG, TAG, "TX overrun!");
						}

						uart_clearbit(LINESTATUS_TRANSMITTERHOLDINGREGISTEREMPTY, &(regs->line_status));
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

				// clear the current id and pending bit
				regs->interrupt_identification &= ~(INTERRUPTIDENTIFY_ID | INTERRUPTIDENTIFY_PENDING);

				if (chan->intpending_highest) {
					regs->interrupt_identification |= (INTERRUPTIDENTIFY_LEVEL_HIGHEST | INTERRUPTIDENTIFY_PENDING);
					chan->intpending_highest = false;
				}

				else if (chan->intpending_second) {
					regs->interrupt_identification |= (INTERRUPTIDENTIFY_LEVEL_SECOND | INTERRUPTIDENTIFY_PENDING);
					chan->intpending_second = false;
				}

				else if (chan->intpending_third) {
					regs->interrupt_identification |= (INTERRUPTIDENTIFY_LEVEL_THIRD | INTERRUPTIDENTIFY_PENDING);
					chan->intpending_third = false;
				}

				else if (chan->intpending_forth) {
					regs->interrupt_identification |= (INTERRUPTIDENTIFY_LEVEL_FORTH | INTERRUPTIDENTIFY_PENDING);
					chan->intpending_forth = false;
				}

				return &(regs->interrupt_identification);
			}
		case UART_REGISTER_LINECONTROL:
			return &(regs->line_control);
		case UART_REGISTER_MODEMCONTROL:
			return &(regs->modem_control);
		case UART_REGISTER_LINESTATUS:
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

	static int clocks = 0;
	clocks++;

	for (int i = 0; i < NUMOFCHANNELS; i++) {

		channel* channel = &(channels[i]);

		if (!uart_bitset(LINECONTROL_DLAB, channel->registers.line_control)) {
			channel->dlabwilllatch = true;
		}

		else {
			if (channel->dlabwilllatch) {
				channel->clockdivider = (channel->registers.divisor_latch_msb << 8)
						+ channel->registers.divisor_latch_lsb;
				log_println(LEVEL_DEBUG, TAG, "Clock divider changed to %d for channel %d", channel->clockdivider, i);
			}
		}

		if (channel->clockdivider != 0) {
			if (clocks % channel->clockdivider != 0) {
				break;
			}
		}
		//log_println(LEVEL_DEBUG, TAG, "Updating channel %d - LS 0x%x", i, channel->registers.line_status);

		// Are we transmitting?
		if (!uart_bitset(LINESTATUS_TRANSMITTEREMPTY, channel->registers.line_status)) {

			log_println(LEVEL_INSANE, TAG, "Transmitting .. clock %d", channel->txclock);

			channel->txclock++;
			// end of byte
			if (channel->txclock == 16) {
				channel->txclock = 0;

				log_println(LEVEL_INSANE, TAG, "Transmitter has finished");

				// transmitter is now empty
				uart_setbit(LINESTATUS_TRANSMITTEREMPTY, &(channel->registers.line_status));
			}
		}

		else {
			//log_println(LEVEL_DEBUG, TAG, "Transmitter is empty!");

			// Check if we have data that is ready to be shifted out
			if (!uart_bitset(LINESTATUS_TRANSMITTERHOLDINGREGISTEREMPTY, channel->registers.line_status)) {

				log_println(LEVEL_INSANE, TAG, "Holding register has something");

				// move data to transmitter .. which is really a cheat.. we just write it to the pty
				write(channel->ptm, &(channel->registers.txfifo[0]), 1);
				log_println(LEVEL_INSANE, TAG, "Sent 0x%02x[%c]", channel->registers.txfifo[0],
						FILTERPRINTABLE(channel->registers.txfifo[0]));

				// We're transmitting
				uart_clearbit(LINESTATUS_TRANSMITTEREMPTY, &(channel->registers.line_status));

				// holding register is now empty
				uart_setbit(LINESTATUS_TRANSMITTERHOLDINGREGISTEREMPTY, &(channel->registers.line_status));

				// TX interrupt

				if (uart_bitset(INTERRUPTENALBE_ETBEI, channel->registers.interrupt_enable)) {
					channel->intpending_third = true;
					board_raise_interrupt(&uartcard);
				}

			}
		}

		// receiver

		int bytes;
		char byte;
		if ((bytes = read(channels[i].ptm, &byte, 1)) != EAGAIN) {
			if (bytes > 0) {
				log_println(LEVEL_DEBUG, TAG, "Read byte 0x%02x[%c] from pty", byte, FILTERPRINTABLE(byte));

				channel->registers.rxfifo[0] = byte;
				if (uart_bitset(LINESTATUS_DATAREADY, channel->registers.line_status)) {
					log_println(LEVEL_INFO, TAG, "RX Overflow");
					uart_setbit(LINESTATUS_OVERRUNERROR, &(channel->registers.line_status));
				}
				uart_setbit(LINESTATUS_DATAREADY, &(channel->registers.line_status));

				// RX interrupt
				if (uart_bitset(INTERRUPTENABLE_ERBFI, channel->registers.interrupt_enable)) {
					channel->intpending_third = true;
					board_raise_interrupt(&uartcard);
				}

			}
		}

	}

}

void uart_irq_ack() {
	log_println(LEVEL_DEBUG, TAG, "uart_irq_ack()");
	board_lower_interrupt(&uartcard);
}

const card uartcard = { "UART CARD", uart_init, uart_dispose, uart_tick, uart_irq_ack, NULL, uart_read_byte, NULL, NULL,
		uart_write_byte, NULL, NULL };
