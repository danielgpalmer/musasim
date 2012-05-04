/**
 *   \file machine.h
 *   \author genheader
 *   \brief Machine helper defines
 */
#ifndef LIBUNAGIPAI_MACHINESTUFF_H_
#define LIBUNAGIPAI_MACHINESTUFF_H_


#include <stdint.h>


#define ROMDISABLE0 0x55
#define ROMDISABLE1 0xaa
#define romdisable0 (*(volatile uint8_t*) 0xaaaaa)
#define romdisable1 (*(volatile uint8_t*) 0x55554)

#endif // MACHINESTUFF
