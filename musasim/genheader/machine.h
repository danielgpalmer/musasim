/* machine.h */

#include <stdint.h>


volatile uint16_t* dma_register_config = (uint16_t*) 0xa00000;
volatile uint16_t* dma_register_data = (uint16_t*) 0xa00002;
volatile uint16_t* dma_register_counth = (uint16_t*) 0xa00004;
volatile uint16_t* dma_register_countl = (uint16_t*) 0xa00006;
volatile uint16_t* dma_register_srch = (uint16_t*) 0xa00008;
volatile uint16_t* dma_register_srcl = (uint16_t*) 0xa0000a;
volatile uint16_t* dma_register_desth = (uint16_t*) 0xa0000c;
volatile uint16_t* dma_register_destl = (uint16_t*) 0xa0000e;

/*
 * dmaregisters.h
 *
 *  Created on: Dec 23, 2011
 *      Author: daniel
 */

#ifndef DMAREGISTERS_H_
#define DMAREGISTERS_H_


#define DMA_REGISTER_CONFIG_SIZE 0x0001 // 0 = 8bits, 1 = 16bits
#define DMA_REGISTER_CONFIG_MODE 0x0002 // 0 = SRC -> DEST, 1 = DATA -> DEST

#define DMA_REGISTER_CONFIG_DACT 0x001c //
#define DMA_REGISTER_CONFIG_SRCACT 0x00e0 //
#define DMA_REGISTER_CONFIG_DSTACT 0x0700 //

#define DMA_ACT_NOTHING 0b000
#define DMA_ACT_INC 0b001
#define DMA_ACT_INCTWO 0b010
#define DMA_ACT_DEC 0b011
#define DMA_ACT_DECTWO 0b100

#define DMA_REGISTER_CONFIG_SRCACT_NOTHING (DMA_ACT_NOTHING << 4)




#define DMA_REGISTER_CONFIG_DONE 0x8000 // 1 when dma transfer finishes
#define DMA_REGISTER_CONFIG_START 0x4000 // write 1 to start dma transfer

#endif /* DMAREGISTERS_H_ */
