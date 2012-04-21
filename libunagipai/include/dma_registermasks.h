/**
 * \file dma_registermasks.h
 * \date Dec 23, 2011
 * \author daniel
 * \brief Register masks for setting up the DMA controller
 */

#ifndef DMAREGISTERS_H_
#define DMAREGISTERS_H_

/*
 *
 *  15	14	13	12	11	10	09	08	07	06	05	04	03	02	01	00
 *  DN	ST	MU	MU	DA	DA	DA  SA	SA	SA	dA	dA	dA	MO	MO	SZ
 *
 *  DN - Done
 *  ST - Start
 *  MU - Mutate
 *  DA - Dest Action
 *  SA - Source action
 *  dA - Data action
 *  MO - Mode
 *  SZ - Size
 */

// size of DMA operation
#define DMA_REGISTER_CONFIG_SIZE 0x0001 // 0 = 8bits, 1 = 16bits
// mode of transfer
#define DMA_REGISTER_CONFIG_MODE_SHIFT 1
#define DMA_REGISTER_CONFIG_MODE (0b11 << DMA_REGISTER_CONFIG_MODE_SHIFT)
#define DMA_REGISTER_CONFIG_MODE_BLOCK (0b00 << DMA_REGISTER_CONFIG_MODE_SHIFT) // Copy data at address in SRC to address in DST
#define DMA_REGISTER_CONFIG_MODE_FILL (0b01 << DMA_REGISTER_CONFIG_MODE_SHIFT)  // Copy value of DATA to address in DST
#define DMA_REGISTER_CONFIG_MODE_MIX (0b10 << DMA_REGISTER_CONFIG_MODE_SHIFT)   // Mutate data at address in SRC with data at address in DATA and copy to DST
#define DMA_REGISTER_CONFIG_MODE_MIXCOMPACT (0b11 << DMA_REGISTER_CONFIG_MODE_SHIFT)   // Mutate data at address in SRC with data at address in DATA and copy to DST
// Actions that will be applied to the latch data,source or destination registers after a single read + write
#define DMA_REGISTER_CONFIG_ACT 0b111
#define DMA_REGISTER_CONFIG_DATAACT_SHIFT 3
#define DMA_REGISTER_CONFIG_DACT (DMA_REGISTER_CONFIG_ACT << DMA_REGISTER_CONFIG_DATAACT_SHIFT) //
#define DMA_REGISTER_CONFIG_SRCACT_SHIFT 6
#define DMA_REGISTER_CONFIG_SRCACT (DMA_REGISTER_CONFIG_ACT << DMA_REGISTER_CONFIG_SRCACT_SHIFT) //
#define DMA_REGISTER_CONFIG_DSTACT_SHIFT 9
#define DMA_REGISTER_CONFIG_DSTACT (DMA_REGISTER_CONFIG_ACT << DMA_REGISTER_CONFIG_DSTACT_SHIFT) //
// Action types
#define DMA_ACT_NOTHING 0b000	// Do nothing
#define DMA_ACT_INC 0b001		// Increment by one
#define DMA_ACT_DEC 0b011		// Decrement by one
#define DMA_ACT_ROTLEFT 0b101 	// Rotate left
#define DMA_ACT_ROTRIGHT 0b110 	// Rotate right
#define DMA_ACT_INVERSE 0b111 	// Invert
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
// Mutators - Apply DATA + A bitwise operator to SRC before writing to DST
#define DMA_REGISTER_CONFIG_MUTATOR_SHIFT 12
#define DMA_REGISTER_CONFIG_MUTATOR (0b11 << DMA_REGISTER_CONFIG_MUTATOR_SHIFT)
#define DMA_MUT_NOTHING (0b00 << DMA_REGISTER_CONFIG_MUTATOR_SHIFT) // Do nothing
#define DMA_MUT_AND (0b01 << DMA_REGISTER_CONFIG_MUTATOR_SHIFT)	// [DATA] and [SRC]
#define DMA_MUT_OR (0b10 << DMA_REGISTER_CONFIG_MUTATOR_SHIFT) // [DATA] or [SRC]
#define DMA_MUT_XOR (0b11 << DMA_REGISTER_CONFIG_MUTATOR_SHIFT) // [DATA] xor [SRC]
// transfer start stop
#define DMA_REGISTER_CONFIG_START 0x4000 // write 1 to start dma transfer
#define DMA_REGISTER_CONFIG_DONE 0x8000 // 1 when dma transfer finishes
#endif /* DMAREGISTERS_H_ */
