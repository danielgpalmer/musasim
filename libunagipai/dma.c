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

void dma_transferblock(uint32_t source, uint32_t dest, uint32_t count) {
	dma_register_counth = HIGHWORD(count);
	dma_register_countl = LOwWORD(count);
	dma_register_desth = HIGHWORD(dest);
	dma_register_destl = LOwWORD(dest);
	dma_register_srch = HIGHWORD(source);
	dma_register_srcl = LOwWORD(source);
	dma_register_config = DMA_REGISTER_CONFIG_START | DMA_REGISTER_CONFIG_SIZE | DMA_REGISTER_CONFIG_MODE_BLOCK
			| DMA_REGISTER_CONFIG_SRCACT_INCTWO | DMA_REGISTER_CONFIG_DSTACT_INCTWO;

	while (!(dma_register_config & DMA_REGISTER_CONFIG_DONE)) {
	}

}

void dma_transferblock_fromregister(uint32_t source, uint32_t dest, uint32_t count) {

	dma_register_counth = HIGHWORD(count);
	dma_register_countl = LOwWORD(count);
	dma_register_desth = HIGHWORD(dest);
	dma_register_destl = LOwWORD(dest);
	dma_register_srch = HIGHWORD(source);
	dma_register_srcl = LOwWORD(source);
	dma_register_config = DMA_REGISTER_CONFIG_START | DMA_REGISTER_CONFIG_SIZE | DMA_REGISTER_CONFIG_MODE_BLOCK
			| DMA_REGISTER_CONFIG_DSTACT_INCTWO;

	while (!(dma_register_config & DMA_REGISTER_CONFIG_DONE)) {
	}

}

void dma_transferblock_toregister(uint32_t source, uint32_t dest, uint32_t count) {

	dma_register_counth = HIGHWORD(count);
	dma_register_countl = LOwWORD(count);
	dma_register_desth = HIGHWORD(dest);
	dma_register_destl = LOwWORD(dest);
	dma_register_srch = HIGHWORD(source);
	dma_register_srcl = LOwWORD(source);
	dma_register_config = DMA_REGISTER_CONFIG_START | DMA_REGISTER_CONFIG_SIZE | DMA_REGISTER_CONFIG_MODE_BLOCK
			| DMA_REGISTER_CONFIG_SRCACT_INCTWO;

	while (!(dma_register_config & DMA_REGISTER_CONFIG_DONE)) {
	}

}

void dma_fillblock(uint32_t dest, uint16_t data, uint32_t count) {
	dma_register_counth = (count >> 16) & 0xFFFF;
	dma_register_countl = (count & 0xFFFF);
	dma_register_desth = 0x0020;
	dma_register_destl = 0x0000;
	dma_register_datal = 0xFFFF;
	//*dma_register_config |= DMA_REGISTER_CONFIG_START | DMA_REGISTER_CONFIG_SIZE | DMA_REGISTER_CONFIG_MODE
	//		| DMA_REGISTER_CONFIG_DATAACT_INVERSE | DMA_REGISTER_CONFIG_DSTACT_INCTWO;

	dma_register_config = DMA_REGISTER_CONFIG_START | DMA_REGISTER_CONFIG_MODE_FILL | DMA_REGISTER_CONFIG_SIZE
			| DMA_REGISTER_CONFIG_DSTACT_INCTWO;

	while (!(dma_register_config & DMA_REGISTER_CONFIG_DONE)) {
	}

}
