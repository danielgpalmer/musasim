/* uart.h */

#include <stdint.h>


#define uart_chan0_rxtx ((volatile uint8_t*) 0x400000)
#define uart_chan0_interruptenable ((volatile uint8_t*) 0x400001)
#define uart_chan0_linecontrol ((volatile uint8_t*) 0x400003)
#define uart_chan0_linestatus ((volatile uint8_t*) 0x400005)

