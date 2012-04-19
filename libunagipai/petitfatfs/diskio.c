/*-----------------------------------------------------------------------*/
/* Low level disk I/O module skeleton for Petit FatFs (C)ChaN, 2009      */
/*-----------------------------------------------------------------------*/

#include "diskio.h"
#include "../include/utils.h"
#include <ata.h>
#include <stdio.h>
#include <string.h>
#include <stdbool.h>

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

static uint32_t lastsector = 0xFFFFFFFF;
static uint16_t buffer[256];

DRESULT disk_readp(uint8_t* dest, uint32_t sector, uint16_t sofs, uint16_t count) {
	printf("disk_readp(0x%08x, 0x%08x, %d, %d)\n", (unsigned) dest, sector, sofs, count);

	if (sofs == 0 && count == 512) {
		printf("directly copying block\n");
		ata_read_sector(sector, dest, true);
	}
	else {
		if (lastsector != sector) {
			ata_read_sector(sector, buffer, true);
			//util_hexblockprint(buffer, 512);
			lastsector = sector;
		}

		uint8_t* bufferbytes = (uint8_t*) buffer;
		memcpy(dest, (void *) (buffer) + sofs, count);
	}
	return RES_OK;
}

DRESULT disk_readp_nocopy(uint8_t** dest, uint32_t sector, uint16_t sofs, uint16_t count) {
	printf("disk_readp_nocopy(0x%08x, 0x%08x, %d, %d)\n", (unsigned) dest, sector, sofs, count);

	if (lastsector != sector) {
		ata_read_sector(sector, buffer, true);
		//util_hexblockprint(buffer, 512);
		lastsector = sector;

	}

	//uint8_t* bufferbytes = (uint8_t*) buffer;
	//memcpy(dest, (void *) (buffer) + sofs, count);

	*dest = (void *) (buffer) + sofs;

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

