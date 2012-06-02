/**
 * \file uart.h
 * \brief Simple uart functions like getch and putch
 */

#ifndef LIBUNAGIPAI_UART_H_
#define LIBUNAGIPAI_UART_H_

#include <stdbool.h>
#include <stdint.h>

char uart_getch(void);
bool uart_getch_nonblock(char* ch);
void uart_putch(char ch);
void uart_putch1(char ch);
bool uart_getch_nonblock1(char* ch);
char uart_getch1(void);
void uart_configureinterrupts(uint8_t channel, bool datareceived, bool transmitterempty, bool linestatus,
		bool modemstatus);
void uart_configurefifos(uint8_t channel, bool enablefifos);

#endif
