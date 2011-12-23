/* machine.h */

#include <stdint.h>


volatile uint16_t* dma_register_config = (uint16_t*) 0xa00000;

/*
 * dmaregisters.h
 *
 *  Created on: Dec 23, 2011
 *      Author: daniel
 */

#ifndef DMAREGISTERS_H_
#define DMAREGISTERS_H_

#define DMA_REGISTER_CONFIG_DONE 0x8000

#endif /* DMAREGISTERS_H_ */
