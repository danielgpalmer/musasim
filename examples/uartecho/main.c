#include "include/uart.h"

int main(void) {
	while (1)
		uart_putch(0, uart_getch(0));
	return 0;
}
