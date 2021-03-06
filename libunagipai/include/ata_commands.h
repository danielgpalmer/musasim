/**
 *	\file	ata_commands.h
 *	\date	Dec 28, 2011
 *	\author	daniel
 *	\brief	ATA command bytes
 */

#ifndef ATACOMMANDS_H_
#define ATACOMMANDS_H_

#define ATA_EXECUTEDRIVEDIAG 0x90
#define ATA_FORMATTRACK 0x50
#define ATA_IDENTIFYDRIVE 0xec
#define ATA_INITIALIZEDRIVEPARAMS 0x91
#define ATA_NOP 0x00
#define ATA_READBUFFER 0xe4

#define ATA_WRITEBUFFER 0xe8

#define ATA_READDMA_RETRY 0xc8
#define ATA_READDMA_NORETRY 0xc9
#define ATA_READLONG_RETRY 0x22
#define ATA_READLONG_NORETRY 0x23
#define ATA_READMULTIPLE 0xc4
#define ATA_READSECTORS_RETRY 0x20
#define ATA_READSECTORS_NORETRY 0x21

#endif /* ATACOMMANDS_H_ */
