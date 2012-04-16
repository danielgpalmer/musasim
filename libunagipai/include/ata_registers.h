/**
 *   \file ata_registers.h
 *   \author genheader
 *   \brief ATA register defines
 */
#ifndef LIBUNAGIPAI_ATAREGISTERS_H_
#define LIBUNAGIPAI_ATAREGISTERS_H_


#include <stdint.h>


#define ata_register_data *((volatile uint16_t*) 0x800000)
#define ata_register_errorfeature *((volatile uint8_t*) 0x800002)
#define ata_register_sectorcount *((volatile uint8_t*) 0x800004)
#define ata_register_sectornumber *((volatile uint8_t*) 0x800006)
#define ata_register_cylinderlow *((volatile uint8_t*) 0x800008)
#define ata_register_cylinderhigh *((volatile uint8_t*) 0x80000a)
#define ata_register_drivehead *((volatile uint8_t*) 0x80000c)
#define ata_register_command *((volatile uint8_t*) 0x80000e)
#define ata_register_altstatus *((volatile uint8_t*) 0x80001c)

#endif // ATAREGISTERS
