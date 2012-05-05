/**
 *   \file uart_registers.h
 *   \author genheader
 *   \brief UART register defines
 */
#ifndef LIBUNAGIPAI_UARTREGISTERS_H_
#define LIBUNAGIPAI_UARTREGISTERS_H_


#include <stdint.h>


#define uart_chan0_rxtx ((volatile uint8_t*) 0x400000)
#define uart_chan0_interruptenable ((volatile uint8_t*) 0x400002)
#define uart_chan0_fifocontrol ((volatile uint8_t*) 0x400004)
#define uart_chan0_linecontrol ((volatile uint8_t*) 0x400006)
#define uart_chan0_linestatus ((volatile uint8_t*) 0x40000a)
#define uart_chan1_rxtx ((volatile uint8_t*) 0x400010)
#define uart_chan1_interruptenable ((volatile uint8_t*) 0x400012)
#define uart_chan1_fifocontrol ((volatile uint8_t*) 0x400014)
#define uart_chan1_linecontrol ((volatile uint8_t*) 0x400016)
#define uart_chan1_linestatus ((volatile uint8_t*) 0x40001a)

#endif // UARTREGISTERS
