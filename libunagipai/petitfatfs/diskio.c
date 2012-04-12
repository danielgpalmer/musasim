/*-----------------------------------------------------------------------*/
/* Low level disk I/O module skeleton for Petit FatFs (C)ChaN, 2009      */
/*-----------------------------------------------------------------------*/

#include "diskio.h"

/*-----------------------------------------------------------------------*/
/* Initialize Disk Drive                                                 */
/*-----------------------------------------------------------------------*/

DSTATUS disk_initialize(void) {
	DSTATUS stat;

	// Put your code here

	return stat;
}

/*-----------------------------------------------------------------------*/
/* Read Partial Sector                                                   */
/*-----------------------------------------------------------------------*/

DRESULT disk_readp(uint8_t* dest, /* Pointer to the destination object */
uint32_t sector, /* Sector number (LBA) */
uint16_t sofs, /* Offset in the sector */
uint16_t count /* Byte count (bit15:destination) */
) {
	DRESULT res;

	// Put your code here

	return res;
}

/*-----------------------------------------------------------------------*/
/* Write Partial Sector                                                  */
/*-----------------------------------------------------------------------*/

DRESULT disk_writep(const uint8_t* buff, /* Pointer to the data to be written, NULL:Initiate/Finalize write operation */
uint32_t sc /* Sector number (LBA) or Number of bytes to send */
) {
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

