/**
 *	\file dma.h
 *	\brief A collection of common DMA operations
 *  \date Apr 10, 2012
 *  \author daniel
 *
 *  A collection of common DMA operations for example block fills, block copies,
 *  block to register (for copying blocks to fifos etc), and register to block
 *  (for reading blocks from fifos etc). For more complex operations program the
 *  DMA controller directly.
 */

#ifndef LIBUNAGIPAI_DMA_H_
#define LIBUNAGIPAI_DMA_H_

/**
 * Transder a block to a block
 */
void dma_transferblock(uint32_t source, uint32_t dest, uint32_t count);

/**
 * Transfer a block of 16bit words from source -> source + (count * 2) to dest.
 */
void dma_transferblock_fromregister(uint32_t source, uint32_t dest, uint32_t count);

/**
 * Transfer a block of 16bit words from source to dest -> dest + (count * 2).
 */
void dma_transferblock_toregister(uint32_t source, uint32_t dest, uint32_t count);

/**
 * Fill a block with data
 */
void dma_fillblock(uint32_t dest, uint16_t data, uint32_t count);

#endif /* LIBUNAGIPAI_DMA_H_ */
