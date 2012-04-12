/*
 * ata.h
 *
 *  Created on: Apr 8, 2012
 *      Author: daniel
 */

#ifndef LIBUNAGIPIE_ATA_H_
#define LIBUNAGIPIE_ATA_H_

#include "ata_idoffsets.h"

typedef struct {
	char serial[(ATA_ID_SERIAL_LEN * 2) + 1];
	char fwrev[(ATA_ID_FIRMWAREVER_LEN * 2) + 1];
	char model[(ATA_ID_MODELNUMBER_LEN * 2) + 1];
} ata_id;

void ata_identify(ata_id* id);
void ata_read_sector();

#endif /* LIBUNAGIPIE_ATA_H_ */
