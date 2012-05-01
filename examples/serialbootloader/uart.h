/**
 * \file uart.h
 * \brief Simple uart functions like getch and putch
 */

#ifndef LIBUNAGIPAI_UART_H_
#define LIBUNAGIPAI_UART_H_

#include <stdbool.h>

void uart_putch(char ch);
bool uart_getch_nonblock(char* ch);
char uart_getch();
void uart_putch1(char ch);
bool uart_getch_nonblock1(char* ch);
char uart_getch1();

#endif
