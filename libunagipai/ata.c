/*
 * ata.c
 *
 *  Created on: Apr 8, 2012
 *      Author: daniel
 */

#include "include/ata.h"
#include "include/ata_registers.h"
#include "include/ata_registermasks.h"
#include "include/ata_commands.h"
#include "include/ata_idoffsets.h"

static void ata_wait() {
	while (ata_register_altstatus & ATA_STATUS_BSY) {
		// spin here
	}
}

static void ata_read() {
	while (ata_register_altstatus & ATA_STATUS_DRQ) {
		uint16_t junk = ata_register_data;
	}
}

void ata_identify() {
	ata_register_command = ATA_IDENTIFYDRIVE;
	ata_wait();
	ata_read();
}
