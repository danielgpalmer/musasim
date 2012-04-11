/* ata_registers.h */

#ifndef LIBUNAGIPAI_ATAREGISTERS_H_
#define LIBUNAGIPAI_ATAREGISTERS_H_


#include <stdint.h>


#define ata_register_command *((volatile uint8_t*) 0x80000e)

#endif // ATAREGISTERS
