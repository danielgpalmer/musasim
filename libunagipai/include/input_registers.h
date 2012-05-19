/**
 *   \file input_registers.h
 *   \author genheader
 *   \brief Input register defines
 */
#ifndef LIBUNAGIPAI_INPUTREGISTERS_H_
#define LIBUNAGIPAI_INPUTREGISTERS_H_


#include <stdint.h>


#define input_port0 (*(volatile uint8_t*) 0xe00000)
#define input_port1 (*(volatile uint8_t*) 0xe00002)
#define input_rng (*(volatile uint8_t*) 0xe00004)

#endif // INPUTREGISTERS
