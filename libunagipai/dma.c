/*
 * dma.c
 *
 *  Created on: Apr 10, 2012
 *      Author: daniel
 */

#include <stdint.h>
#include "include/dma.h"
#include "include/dma_registers.h"
#include "include/dma_registermasks.h"

#define HIGHWORD(a) ((a >> 16) & 0xFFFF)
#define LOwWORD(a) (a & 0xFFFF)

void dma_transferblock_toregister(uint32_t source, uint32_t dest, uint32_t count) {

	dma_register_counth = HIGHWORD(count);
	dma_register_countl = LOwWORD(count);
	dma_register_desth = HIGHWORD(dest);
	dma_register_destl = LOwWORD(dest);
	dma_register_srch = HIGHWORD(source);
	dma_register_srcl = LOwWORD(source);
	dma_register_config = DMA_REGISTER_CONFIG_START | DMA_REGISTER_CONFIG_SIZE | DMA_REGISTER_CONFIG_MODE_BLOCK
			| DMA_REGISTER_CONFIG_SRCACT_INCTWO;

}
