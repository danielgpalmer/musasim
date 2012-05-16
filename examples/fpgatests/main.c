/*
 * main.c
 *
 *  Created on: Jul 17, 2011
 *      Author: daniel
 */

#include <stdio.h>
#include <stdint.h>
#include "uart.h"
#include "uart_registers.h"
#include "uart_registermasks.h"


int main(void) {


	//*uart_chan0_linecontrol |= LINECONTROL_DLAB;
	//*uart_chan0_rxtx = 0x01;
	//*uart_chan0_interruptenable = 0x00;
	//*uart_chan0_linecontrol &= ~LINECONTROL_DLAB;

	uart_chan0_modemcontrol = 0x07;
	

	while(1){
		//*uart_chan0_modemcontrol |= 0x0C;
		//for(int foo = 0; foo < 0xFF; foo++){
  		//	for(int bar = 0; bar < 0xFF; bar++){
		//		asm volatile ("nop \n\t");
		//	}
 		//}
		//for(int i = 0; i < 5; i++){
		//	*uart_chan0_linecontrol = 0x03;
			uart_putch('h');
		//}
		//*uart_chan0_modemcontrol &= ~0x0C;
		//for(int foo = 0; foo < 0xFF; foo++){
  		//	for(int bar = 0; bar < 0xFF; bar++){
		//		asm volatile ("nop \n\t");
		//	}
 		//}


	}
	return 0;
}
