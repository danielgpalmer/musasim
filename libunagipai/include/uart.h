/**
 * \file uart.h
 * \brief Simple uart functions like getch and putch
 */

#ifndef LIBUNAGIPAI_UART_H_
#define LIBUNAGIPAI_UART_H_

#include <stdbool.h>
#include <stdint.h>

/**
 * get a character from the uart, block if there isn't anything to get yet
 */

char uart_getch(uint8_t channel);

/**
 * get a character from the uart but don't block if there isn't anything to get
 */

bool uart_getch_nonblock(uint8_t channel, char* ch);

/**
 * send a character
 */

void uart_putch(uint8_t channel, char ch);

/**
 * configure the interrupts that the uart should fire
 */

void uart_configureinterrupts(uint8_t channel, bool datareceived, bool transmitterempty, bool linestatus,
		bool modemstatus);

/**
 * enable or disable the uarts RX/TX fifos
 */

void uart_configurefifos(uint8_t channel, bool enablefifos);

#endif
