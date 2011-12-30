/*
 * dmaregisters.h
 *
 *  Created on: Dec 23, 2011
 *      Author: daniel
 */

#ifndef DMAREGISTERS_H_
#define DMAREGISTERS_H_

#define DMA_REGISTER_CONFIG_DATAACT_SHIFT 3
#define DMA_REGISTER_CONFIG_SRCACT_SHIFT 6
#define DMA_REGISTER_CONFIG_DSTACT_SHIFT 9
#define DMA_REGISTER_CONFIG_ACT 0b111

#define DMA_REGISTER_CONFIG_SIZE 0x0001 // 0 = 8bits, 1 = 16bits
#define DMA_REGISTER_CONFIG_MODE 0x0006 // 00 = SRC -> DEST, 01 = DATA(IMM lower 16bits will be used) -> DEST, 10 LOAD DATA, SRC -> DEST
#define DMA_REGISTER_CONFIG_DACT (DMA_REGISTER_CONFIG_ACT << DMA_REGISTER_CONFIG_DATAACT_SHIFT) //
#define DMA_REGISTER_CONFIG_SRCACT (DMA_REGISTER_CONFIG_ACT << DMA_REGISTER_CONFIG_SRCACT_SHIFT) //
#define DMA_REGISTER_CONFIG_DSTACT (DMA_REGISTER_CONFIG_ACT << DMA_REGISTER_CONFIG_DSTACT_SHIFT) //
// Mutators - Apply DATA + A bitwise operator to SRC before writing to DST
#define DMA_REGISTER_CONFIG_MUTATOR 0x1800
#define DMA_MUT_NOTHING (0b00 << 11)
#define DMA_MUT_AND (0b01 << 11)
#define DMA_MUT_OR (0b10 << 11)
#define DMA_MUT_XOR (0b11 << 11)

#define DMA_REGISTER_CONFIG_DONE 0x8000 // 1 when dma transfer finishes
#define DMA_REGISTER_CONFIG_START 0x4000 // write 1 to start dma transfer
// Actions that will be applied to the latch data,source or destination registers after a single read + write
#define DMA_ACT_NOTHING 0b000
#define DMA_ACT_INC 0b001
#define DMA_ACT_INCTWO 0b010
#define DMA_ACT_DEC 0b011
#define DMA_ACT_DECTWO 0b100
#define DMA_ACT_ROTLEFT 0b101 	//
#define DMA_ACT_ROTRIGHT 0b110 	// Only for data?
#define DMA_ACT_INVERSE 0b111 	//
// Bit masks for the data actions
#define DMA_REGISTER_CONFIG_DATAACT_NOTHING (DMA_ACT_NOTHING << DMA_REGISTER_CONFIG_DATAACT_SHIFT)
#define DMA_REGISTER_CONFIG_DATAACT_INC (DMA_ACT_INC << DMA_REGISTER_CONFIG_DATAACT_SHIFT)
#define DMA_REGISTER_CONFIG_DATAACT_INCTWO (DMA_ACT_INCTWO << DMA_REGISTER_CONFIG_DATAACT_SHIFT)
#define DMA_REGISTER_CONFIG_DATAACT_DEC (DMA_ACT_DEC << DMA_REGISTER_CONFIG_DATAACT_SHIFT)
#define DMA_REGISTER_CONFIG_DATAACT_DECTWO (DMA_ACT_DECTWO << DMA_REGISTER_CONFIG_DATAACT_SHIFT)
#define DMA_REGISTER_CONFIG_DATAACT_ROTLEFT (DMA_ACT_ROTLEFT << DMA_REGISTER_CONFIG_DATAACT_SHIFT)
#define DMA_REGISTER_CONFIG_DATAACT_ROTRIGHT (DMA_ACT_ROTRIGHT << DMA_REGISTER_CONFIG_DATAACT_SHIFT)
#define DMA_REGISTER_CONFIG_DATAACT_INVERSE (DMA_ACT_INVERSE << DMA_REGISTER_CONFIG_DATAACT_SHIFT)

// Bit masks for the src actions
#define DMA_REGISTER_CONFIG_SRCACT_NOTHING (DMA_ACT_NOTHING << DMA_REGISTER_CONFIG_SRCACT_SHIFT)
#define DMA_REGISTER_CONFIG_SRCACT_INC (DMA_ACT_INC << DMA_REGISTER_CONFIG_SRCACT_SHIFT)
#define DMA_REGISTER_CONFIG_SRCACT_INCTWO (DMA_ACT_INCTWO <<DMA_REGISTER_CONFIG_SRCACT_SHIFT)
#define DMA_REGISTER_CONFIG_SRCACT_DEC (DMA_ACT_DEC << DMA_REGISTER_CONFIG_SRCACT_SHIFT)
#define DMA_REGISTER_CONFIG_SRCACT_TWO (DMA_ACT_DECTWO << DMA_REGISTER_CONFIG_SRCACT_SHIFT)

// Bit masks for the dest actions
#define DMA_REGISTER_CONFIG_DSTACT_NOTHING (DMA_ACT_NOTHING << DMA_REGISTER_CONFIG_DSTACT_SHIFT)
#define DMA_REGISTER_CONFIG_DSTACT_INC (DMA_ACT_INC << DMA_REGISTER_CONFIG_DSTACT_SHIFT)
#define DMA_REGISTER_CONFIG_DSTACT_INCTWO (DMA_ACT_INCTWO << DMA_REGISTER_CONFIG_DSTACT_SHIFT)
#define DMA_REGISTER_CONFIG_DSTACT_DEC (DMA_ACT_DEC << DMA_REGISTER_CONFIG_DSTACT_SHIFT)
#define DMA_REGISTER_CONFIG_DSTACT_TWO (DMA_ACT_DECTWO << DMA_REGISTER_CONFIG_DSTACT_SHIFT)
#endif /* DMAREGISTERS_H_ */
