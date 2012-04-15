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

#include "include/dma.h"

#include <stdint.h>
#include <stdbool.h>
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

static void ata_read_buffer_dma(uint16_t* buffer) {
	dma_transferblock_toregister(0x800000, buffer, 256);
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

void ata_read_sector(uint32_t sector, uint16_t* buffer, bool dma) {
	memset(buffer, 0x00, 512);
	ata_register_drivehead = (uint8_t) 0xF0 | ((sector >> 24) & 0x0F);
	ata_register_sectornumber = (uint8_t)(sector & 0xff);
	ata_register_cylinderlow = (uint8_t)((sector >> 8) & 0xff);
	ata_register_cylinderhigh = (uint8_t)((sector >> 16) & 0xff);
	ata_register_command = ATA_READBUFFER;
	ata_wait();
	if (dma) {
		ata_read_buffer_dma(buffer);
	}
	else {
		ata_read_buffer(buffer);
	}
}

void ata_write_sector(uint32_t sector, uint16_t* buffer, bool dma) {

}
