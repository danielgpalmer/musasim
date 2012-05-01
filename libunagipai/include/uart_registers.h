/**
 *   \file uart_registers.h
 *   \author genheader
 *   \brief UART register defines
 */
#ifndef LIBUNAGIPAI_UARTREGISTERS_H_
#define LIBUNAGIPAI_UARTREGISTERS_H_


#include <stdint.h>


#define uart_chan0_rxtx ((volatile uint8_t*) 0x400000)
#define uart_chan0_interruptenable ((volatile uint8_t*) 0x400001)
#define uart_chan0_fifocontrol ((volatile uint8_t*) 0x400002)
#define uart_chan0_linecontrol ((volatile uint8_t*) 0x400003)
#define uart_chan0_linestatus ((volatile uint8_t*) 0x400005)
#define uart_chan1_rxtx ((volatile uint8_t*) 0x400008)
#define uart_chan1_interruptenable ((volatile uint8_t*) 0x400009)
#define uart_chan1_fifocontrol ((volatile uint8_t*) 0x40000a)
#define uart_chan1_linecontrol ((volatile uint8_t*) 0x40000b)
#define uart_chan1_linestatus ((volatile uint8_t*) 0x40000d)

#endif // UARTREGISTERS
