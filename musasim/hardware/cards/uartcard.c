/*
 * uartcard.c
 *
 *  Created on: Nov 26, 2011
 *      Author: daniel
 */

#define _XOPEN_SOURCE 600 // to get the pts shizzle
#define _BSD_SOURCE // for cfmakeraw
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include <fcntl.h>
#include <termios.h>
#include "uartcard.h"
#include <uart_registermasks.h>
#include <unistd.h>
#include <errno.h>
#include <glib.h>

#include "../../logging.h"

#include "../board.h"

#define FIFOSIZE 16
#define NUMOFCHANNELS 2

#define MAXLOGBUFFER 2048
static bool loggingenabled = false;
static uint8_t* logbuffer;
static unsigned bufferedchars = 0;

static const char TAG[] = "uart";

typedef struct {
	uint8_t rxbyte;
	uint8_t txbyte;
	uint8_t txshift;
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
	GQueue* txfifo;
	GQueue* rxfifo;
} channel;

static channel channels[NUMOFCHANNELS];

static void uart_clearbit(uint8_t mask, uint8_t* target) {
	*target &= ~mask;
}

static void uart_setbit(uint8_t mask, uint8_t* target) {
	*target |= mask;
}

static bool uart_bitset(uint8_t mask, uint8_t target) {
	return (target & mask) == mask ? true : false;
}

static void uart_reset_channel(channel* chan) {
	memset(&(chan->registers), 0, sizeof(registers));
	chan->registers.line_status = 0x60;
	chan->registers.interrupt_identification = 0x01;

	chan->txclock = 0;
	chan->clockdivider = 0;
	g_queue_clear(chan->txfifo);
	g_queue_clear(chan->rxfifo);
}

static void uart_init() {

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

		channels[i].txfifo = g_queue_new();
		channels[i].rxfifo = g_queue_new();
		uart_reset_channel(&(channels[i]));
	}
}

static void uart_dispose() {

	for (int i = 0; i < NUMOFCHANNELS; i++) {
		close(channels[i].ptm);
		if (channels[i].rxfifo != NULL ) {
			g_queue_free(channels[i].rxfifo);
		}
		if (channels[i].txfifo != NULL ) {
			g_queue_free(channels[i].txfifo);
		}
	}

	if (logbuffer != NULL ) {
		free(logbuffer);
	}

}

/*
 * A4		->	Channel
 * A3 - A1	->	A2 - A0
 *
 */

#define CHANNELMASK 0x10
#define REGISTERMASK 0x0F

static uint8_t* uart_decode_register(uint32_t address, bool write) {
	channel* chan = &(channels[(address & CHANNELMASK) >> 4]);
	registers* regs = &(chan->registers);
	uint8_t reg = (address & REGISTERMASK);

	switch (reg) {
		case UART_REGISTER_RXTXBUFFER:
			if (uart_bitset(LINECONTROL_DLAB, regs->line_control)) {
				return &(regs->divisor_latch_lsb);
			}
			else {
				if (write) {
					if (uart_bitset(FIFOCONTROL_ENABLE, regs->fifo_control)) {
						uint8_t* byte = malloc(1);
						g_queue_push_head(chan->txfifo, byte);
						uart_clearbit(LINESTATUS_TRANSMITTERHOLDINGREGISTEREMPTY, &(regs->line_status)); // datasheet says this is cleared when at least one byte is in the xmit fifo
						return byte;
					}
					else {
						log_println(LEVEL_INSANE, TAG, "Host wrote a byte");
						if (!uart_bitset(LINESTATUS_TRANSMITTERHOLDINGREGISTEREMPTY, regs->line_status)) {
							log_println(LEVEL_DEBUG, TAG, "TX overrun!");
						}

						uart_clearbit(LINESTATUS_TRANSMITTERHOLDINGREGISTEREMPTY, &(regs->line_status));
						return &(regs->txbyte);
					}
				}
				else {
					if (uart_bitset(FIFOCONTROL_ENABLE, regs->fifo_control)) {

						uint8_t* byte = NULL;
						if (g_queue_get_length(chan->rxfifo)) {
							byte = g_queue_pop_tail(chan->rxfifo);
						}
						else {
							log_println(LEVEL_DEBUG, TAG, "FIFO IS EMPTY!!!");
							byte = malloc(1);
							byte = 0;
						}

						if (g_queue_is_empty(chan->rxfifo)) {
							//log_println(LEVEL_DEBUG, TAG, "fifo is dry");
							uart_clearbit(LINESTATUS_DATAREADY, &(regs->line_status));
						}

						return byte;
					}
					else {
						uart_clearbit(LINESTATUS_DATAREADY, &(regs->line_status));
						return &(regs->rxbyte);
					}
				}
			}
		case UART_REGISTER_INTERRUPTENABLE:
			if (uart_bitset(LINECONTROL_DLAB, regs->line_control)) {
				return &(regs->divisor_latch_msb);
			}
			else {
				return &(regs->interrupt_enable);
			}
		case UART_REGISTER_FIFOCONTROL:
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
		case UART_REGISTER_MODEMSTATUS:
			return &(regs->modem_status);
		case UART_REGISTER_SCRATCH:
			return &(regs->scratch);
		default:
			return NULL ;
	}

}

static uint8_t uart_read_byte(uint32_t address) {
	uint8_t* reg = uart_decode_register(address, false);
	return *reg;
}

static void uart_write_byte(uint32_t address, uint8_t value) {
	uint8_t* reg = uart_decode_register(address, true);
	*reg = value;
}

static void uart_log_ch(uint8_t ch) {

	if (loggingenabled) {
		if (ch == '\n' || bufferedchars == MAXLOGBUFFER) {
			logbuffer[bufferedchars] = 0;
			log_println(LEVEL_INFO, TAG, "%s", logbuffer);
			bufferedchars = 0;
		}
		else {
			if (ch >= 0x20 && ch <= 0x7e) { // limit this to printable chars
				logbuffer[bufferedchars++] = ch;
			}
		}
	}

}

void uart_enable_logging() {
	logbuffer = malloc(MAXLOGBUFFER + 1);
	loggingenabled = true;
}

static void uart_tick(int cyclesexecuted) {

	static int clocks = 0;
	clocks++;

	for (int i = 0; i < NUMOFCHANNELS; i++) {

		channel* channel = &(channels[i]);

		// fifo reset logic
		if (uart_bitset(FIFOCONTROL_RCVRFIFORESET, channel->registers.fifo_control)) {
			g_queue_clear(channel->rxfifo);
			uart_clearbit(LINESTATUS_DATAREADY, &(channel->registers.line_status));
			uart_clearbit(FIFOCONTROL_RCVRFIFORESET, &(channel->registers.fifo_control));
			log_println(LEVEL_DEBUG, TAG, "rx fifo cleared for channel %d", i);
		}

		if (uart_bitset(FIFOCONTROL_XMITFIFORESET, channel->registers.fifo_control)) {
			g_queue_clear(channel->txfifo);
			uart_clearbit(FIFOCONTROL_XMITFIFORESET, &(channel->registers.fifo_control));
			log_println(LEVEL_DEBUG, TAG, "tx fifo cleared for channel %d", i);
		}
		//

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

				if (uart_bitset(FIFOCONTROL_ENABLE, channel->registers.fifo_control)) {
					uint8_t* byte = g_queue_pop_tail(channel->txfifo);
					write(channel->ptm, byte, 1);
					if (i == 0) {
						uart_log_ch(*byte);
					}

					if (g_queue_is_empty(channel->txfifo)) {
						// holding register is now empty
						uart_setbit(LINESTATUS_TRANSMITTERHOLDINGREGISTEREMPTY, &(channel->registers.line_status));
					}

				}
				else {
					// move data to transmitter .. which is really a cheat.. we just write it to the pty
					write(channel->ptm, &(channel->registers.txbyte), 1);
					if (i == 0) {
						uart_log_ch(channel->registers.txbyte);
					}

					// holding register is now empty
					uart_setbit(LINESTATUS_TRANSMITTERHOLDINGREGISTEREMPTY, &(channel->registers.line_status));

					// TX interrupt

					if (uart_bitset(INTERRUPTENALBE_ETBEI, channel->registers.interrupt_enable)) {
						channel->intpending_third = true;
						board_raise_interrupt(&uartcard);
					}
				}
				// We're transmitting
				uart_clearbit(LINESTATUS_TRANSMITTEREMPTY, &(channel->registers.line_status));

			}
		}

		// receiver

		int bytes;
		uint8_t byte;
		if ((bytes = read(channels[i].ptm, &byte, 1)) != EAGAIN) {
			if (bytes > 0) {
				//log_println(LEVEL_DEBUG, TAG, "Read byte 0x%02x[%c] from pty", byte, FILTERPRINTABLE(byte));

				if (uart_bitset(FIFOCONTROL_ENABLE, channel->registers.fifo_control)) {
					uint8_t* b = malloc(1);
					*b = byte;
					g_queue_push_head(channel->rxfifo, b);
					//log_println(LEVEL_DEBUG, TAG, "byte into fifo on channel %d, have %u bytes", i,
					//		g_queue_get_length(channel->rxfifo));

				}
				else {
					channel->registers.rxbyte = byte;
					if (uart_bitset(LINESTATUS_DATAREADY, channel->registers.line_status)) {
						log_println(LEVEL_INFO, TAG, "RX Overflow on channel %d", i);
						uart_setbit(LINESTATUS_OVERRUNERROR, &(channel->registers.line_status));
					}
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

static void uart_irq_ack() {
	log_println(LEVEL_DEBUG, TAG, "uart_irq_ack()");
	board_lower_interrupt(&uartcard);
}

static const bool uart_validaddress(uint32_t address) {

	if (address & 0x1) {
		// uart datalines are only attached to d15 - d8
		return false;
	}

	uint8_t reg = (address & REGISTERMASK);

	switch (reg) {
		case UART_REGISTER_RXTXBUFFER:
		case UART_REGISTER_INTERRUPTENABLE:
		case UART_REGISTER_FIFOCONTROL:
		case UART_REGISTER_LINECONTROL:
		case UART_REGISTER_MODEMCONTROL:
		case UART_REGISTER_LINESTATUS:
		case UART_REGISTER_MODEMSTATUS:
		case UART_REGISTER_SCRATCH:
			return true;
		default:
			return false;

	}

}

const card uartcard = { "UART CARD", //
		uart_init, //
		uart_dispose, //
		NULL, //
		uart_tick, //
		NULL, // pause
		uart_irq_ack, //
		NULL, //
		NULL, //
		uart_validaddress, //
		NULL, //
		uart_read_byte, //
		NULL, //
		NULL, //
		uart_write_byte, //
		NULL, //
		NULL, //
		NULL, //
		NULL, //
		NULL, //
		NULL //
};
