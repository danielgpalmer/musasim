/**
 *   \file dma_registers.h
 *   \author genheader
 *   \brief DMA register defines
 */
#ifndef LIBUNAGIPAI_DMAREGISTERS_H_
#define LIBUNAGIPAI_DMAREGISTERS_H_


#include <stdint.h>


#define dma_register_config (*(volatile uint16_t*) 0xa00000)
#define dma_register_datah (*(volatile uint16_t*) 0xa00002)
#define dma_register_datal (*(volatile uint16_t*) 0xa00004)
#define dma_register_counth (*(volatile uint16_t*) 0xa00006)
#define dma_register_countl (*(volatile uint16_t*) 0xa00008)
#define dma_register_srch (*(volatile uint16_t*) 0xa0000a)
#define dma_register_srcl (*(volatile uint16_t*) 0xa0000c)
#define dma_register_desth (*(volatile uint16_t*) 0xa0000e)
#define dma_register_destl (*(volatile uint16_t*) 0xa00010)

#endif // DMAREGISTERS
