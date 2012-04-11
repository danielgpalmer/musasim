/*
 * ata.c
 *
 *  Created on: Apr 8, 2012
 *      Author: daniel
 */

#include "include/ata.h"
#include "include/ata_registers.h"
#include "include/ata_commands.h"
#include "include/ata_idoffsets.h"

void ata_identify() {
	ata_register_command = ATA_IDENTIFYDRIVE;
}
