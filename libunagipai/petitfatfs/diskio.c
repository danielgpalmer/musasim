/*-----------------------------------------------------------------------*/
/* Low level disk I/O module skeleton for Petit FatFs (C)ChaN, 2009      */
/*-----------------------------------------------------------------------*/

#include "diskio.h"
#include "../include/utils.h"
#include <ata.h>
#include <stdio.h>
#include <string.h>

/*-----------------------------------------------------------------------*/
/* Initialize Disk Drive                                                 */
/*-----------------------------------------------------------------------*/

DSTATUS disk_initialize(void) {
	printf("disk_initialize()\n");
	DSTATUS stat;
	ata_id id;
	ata_identify(&id);

	return stat;
}

/*-----------------------------------------------------------------------*/
/* Read Partial Sector                                                   */
/*-----------------------------------------------------------------------*/

/* Pointer to the destination object */
/* Sector number (LBA) */
/* Offset in the sector */
/* Byte count (bit15:destination) */
DRESULT disk_readp(uint8_t* dest, uint32_t sector, uint16_t sofs, uint16_t count) {
	printf("disk_readp(0x%08x, 0x%08x, %d, %d)\n", (unsigned) dest, sector, sofs, count);
	uint16_t buffer[256];
	uint8_t* bufferbytes = (uint8_t*) buffer;
	ata_read_sector(buffer);
	memcpy(dest, (void *) (buffer) + sofs, count);

	util_hexblockprint(buffer, sizeof(buffer));

	for (int i = 0; i < count; i++) {
		printf("0x%02x ", (unsigned) dest[i]);
	}
	printf("\n");

	return RES_OK;
}

/*-----------------------------------------------------------------------*/
/* Write Partial Sector                                                  */
/*-----------------------------------------------------------------------*/

/* Pointer to the data to be written, NULL:Initiate/Finalize write operation */
/* Sector number (LBA) or Number of bytes to send */
DRESULT disk_writep(const uint8_t* buff, uint32_t sc) {
	printf("disk_writep(0x%08x, %d)\n", buff, sc);
	DRESULT res;

	if (!buff) {
		if (sc) {

			// Initiate write process

		}
		else {

			// Finalize write process

		}
	}
	else {

		// Send data to the disk

	}

	return res;
}

