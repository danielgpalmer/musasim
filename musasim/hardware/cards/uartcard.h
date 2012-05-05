/*
 * uartcard.h
 *
 *  Created on: Nov 26, 2011
 *      Author: daniel
 */

#ifndef UARTCARD_H_
#define UARTCARD_H_

#include "card.h"
const card uartcard;

#define UART_REGISTER_RXTXBUFFER 		0
#define UART_REGISTER_INTERRUPTENABLE	(1 << 1)
#define UART_REGISTER_FIFOCONTROL 		(2 << 1)
#define UART_REGISTER_LINECONTROL 		(3 << 1)
#define UART_REGISTER_MODEMCONTROL 		(4 << 1)
#define UART_REGISTER_LINESTATUS		(5 << 1)
#define UART_REGISTER_MODEMSTATUS		(6 << 1)
#define UART_REGISTER_SCRATCH			(7 << 1)

void uart_enable_logging();

#endif /* UARTCARD_H_ */
