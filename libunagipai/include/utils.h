/*
 * utils.h
 *
 *  Created on: Apr 13, 2012
 *      Author: daniel
 */

#ifndef UTILS_H_
#define UTILS_H_

#include "pff.h"

#define SIZEOFARRAY(a) (sizeof(a)/sizeof(a[0]))
#define MAX(a,b) ((a) > (b) ? a : b)
#define MIN(a,b) ((a) < (b) ? a : b)
#define CLAMP(min,max, value) (MAX(MIN(max,value), min))

/**
 * prints a block of bytes as hex using printf
 */
void util_hexblockprint(void* buffer, unsigned len);
void util_printfat(FATFS* fs);
void util_printffresult(FRESULT result);

#endif /* UTILS_H_ */
