/*
 * utils.c
 *
 *  Created on: Apr 13, 2012
 *      Author: daniel
 */

#include <stdint.h>
#include <stdio.h>

void util_hexblockprint(void* buffer, unsigned len) {

	uint8_t* bytes = (uint8_t*) buffer;

	printf("len is %d\n", len);

	for (unsigned i = 0; i < len; i += 0x10) {
		printf("%x \t- ", (unsigned) i);
		for (unsigned j = 0; j < 0x10; j++) {
			printf("0x%02x ", (unsigned) bytes[i + j]);
		}
		printf("\n");
	}

}
