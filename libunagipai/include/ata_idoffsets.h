/**
 * \file ata_idoffsets.h
 * \date Dec 28, 2011
 * \author daniel
 * \brief Offsets within the ATA ID block
 */

// All lengths are 1 unless defined .. units are 16bit words not bytes!!
#ifndef ATAIDOFFSETS_H_
#define ATAIDOFFSETS_H_

#define ATA_ID_TOTALLEN 512

#define ATA_ID_INFO 0
#define ATA_ID_CYL 1
#define ATA_ID_HEADS 3
#define ATA_ID_SERIAL 10
#define ATA_ID_SERIAL_LEN 10

#define ATA_ID_FIRMWAREVER 23
#define ATA_ID_FIRMWAREVER_LEN 3

#define ATA_ID_MODELNUMBER 27
#define ATA_ID_MODELNUMBER_LEN 20

#define ATA_ID_LBASECTORS 60
#define ATA_ID_LBASECTORS_LEN 2

#endif /* ATAIDOFFSETS_H_ */
