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

#include <string.h>

static void ata_wait() {
	while (ata_register_altstatus & ATA_STATUS_BSY) {
		// spin here
	}
}

static void ata_read_buffer(uint16_t* buffer) {
	uint16_t word = 0;
	while (ata_register_altstatus & ATA_STATUS_DRQ) {
		buffer[word++] = ata_register_data;
	}
}

void ata_identify(ata_id* id) {
	uint16_t buffer[256];
	ata_register_command = ATA_IDENTIFYDRIVE;
	ata_wait();
	ata_read_buffer(buffer);

	memset(id, 0, sizeof(ata_id));
	for (int word = 0; word < ATA_ID_SERIAL_LEN; word++) {
		id->serial[(word * 2)] = buffer[ATA_ID_SERIAL + word] & 0xFF;
		id->serial[(word * 2) + 1] = (buffer[ATA_ID_SERIAL + word] >> 8) & 0xFF;
	}

	for (int word = 0; word < ATA_ID_FIRMWAREVER_LEN; word++) {
		id->fwrev[(word * 2)] = buffer[ATA_ID_FIRMWAREVER + word] & 0xFF;
		id->fwrev[(word * 2) + 1] = (buffer[ATA_ID_FIRMWAREVER + word] >> 8) & 0xFF;
	}

	for (int word = 0; word < ATA_ID_MODELNUMBER_LEN; word++) {
		id->model[(word * 2)] = buffer[ATA_ID_MODELNUMBER + word] & 0xFF;
		id->model[(word * 2) + 1] = (buffer[ATA_ID_MODELNUMBER + word] >> 8) & 0xFF;
	}
}

void ata_read_sector() {
	uint16_t buffer[256];
	ata_register_command = ATA_READBUFFER;
	ata_wait();
	ata_read_buffer(buffer);
}

void ata_write_sector() {

}
