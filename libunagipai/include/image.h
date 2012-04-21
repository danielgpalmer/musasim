/*
 * image.h
 *
 *  Created on: Apr 21, 2012
 *      Author: daniel
 */

#ifndef IMAGE_H_
#define IMAGE_H_

#include <stdbool.h>
#include <stdint.h>
#include "pff.h"

uint8_t* image_loadimagefromfile(FATFS* fs, const char* path,  uint16_t* width, uint16_t* height,bool compressed);

#endif /* IMAGE_H_ */
