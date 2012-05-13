/*
 * main.c
 *
 *  Created on: Jul 17, 2011
 *      Author: daniel
 */

#include <stdio.h>
#include <stdint.h>

volatile uint8_t anarray[256];

int main(void) {

	for(int i = 0; i < 256; i++){
	 anarray[i] = i;
	}


	while(1){
	//	printf("Hello, world!\n");
	}
	return 0;
}
