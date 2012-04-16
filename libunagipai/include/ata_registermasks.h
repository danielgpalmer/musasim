/**
 * \file ata_registermasks.h
 * \date Apr 11, 2012
 * \author daniel
 * \brief Register masks for the CF interface
 */

#ifndef ATA_REGISTERMASKS_H_
#define ATA_REGISTERMASKS_H_

#define ATA_STATUS_BSY 0x80
#define ATA_STATUS_DRDY 0x40
#define ATA_STATUS_DWF 0x20
#define ATA_STATUS_DSC 0x10
#define ATA_STATUS_DRQ 0x08
#define ATA_STATUS_CORR 0x04
#define ATA_STATUS_IDX 0x02
#define ATA_STATUS_ERR 0x01

#endif /* ATA_REGISTERMASKS_H_ */
