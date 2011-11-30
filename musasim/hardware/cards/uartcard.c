/*
 * uartcard.c
 *
 *  Created on: Nov 26, 2011
 *      Author: daniel
 */

#include <stdint.h>
#include "uartcard.h"

#define FIFOSIZE 16
#define NUMOFCHANNELS 2

typedef struct {
	uint8_t receiver_buffer;
	uint8_t transmitter_buffer;
	uint8_t interrupt_enable;
	uint8_t interrupt_identification;
	uint8_t fifo_control;
	uint8_t line_control;
	uint8_t modem_status;
	uint8_t scratch;
	uint8_t divisor_latch;
} registers;

typedef struct {
	registers registers;
	uint8_t rxfifo[FIFOSIZE];
	uint8_t txfifo[FIFOSIZE];
} channel;

channel channels[NUMOFCHANNELS];

uint8_t uart_read_byte(uint32_t address) {

}

void uart_write_byte(uint32_t address, uint8_t value) {

}

card uartcard = { "UART CARD", uart_read_byte, uart_write_byte, NULL, NULL, NULL, NULL, NULL };
