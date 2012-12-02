/*
 * utils.h
 *
 *  Created on: Apr 13, 2012
 *      Author: daniel
 */

#ifndef UTILS_H_
#define UTILS_H_

#include "pff.h"

/**
 * prints a block of bytes as hex using printf
 */
void util_hexblockprint(void* buffer, unsigned len);
void util_printfat(FATFS* fs);
void util_printffresult(FRESULT result);

#endif /* UTILS_H_ */
