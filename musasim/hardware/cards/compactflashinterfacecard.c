/*
 * compactflashinterfacecard.c
 *
 *  Created on: Dec 3, 2011
 *      Author: daniel
 */

#include "compactflashinterfacecard.h"
#include "atacommands.h"
#include "ataidoffsets.h"

#include <errno.h>
#include <fcntl.h>
#include <stdint.h>
#include <stdbool.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <unistd.h>
#include <string.h>
#include "../../logging.h"
#include <stdlib.h>

static const char TAG[] = "cfint";

typedef struct {
	uint16_t data;
	uint8_t error;
	uint8_t feature;
	uint8_t sectorcount;
	uint8_t sectornumber;
	uint8_t cylinderhigh;
	uint8_t cylinderlow;
	uint8_t drivehead;
	uint8_t status;
	uint8_t command;
} taskfile;

typedef struct {
	uint8_t altstatus;
	uint8_t devicecontrol;
	uint8_t driveaddress;
} control;

static taskfile tf;
static control c;

static int fd;
static uint8_t* idblock;
static uint8_t* image;
static size_t size;

bool cfintf_load(const char* filename) {
	log_println(LEVEL_INFO, TAG, "Loading %s", filename);

	fd = open(filename, O_RDWR);

	if (fd < 0) {
		log_println(LEVEL_WARNING, TAG, "Failed to open %s", filename);
		return false;
	}

	struct stat filestat;
	fstat(fd, &filestat);

	size = filestat.st_size;

	log_println(LEVEL_INFO, TAG, "Image is %d bytes", size);

	if ((image = mmap(NULL, size, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0)) == MAP_FAILED) {
		log_println(LEVEL_WARNING, TAG, "Failed to map image, %d", errno);
		return false;
	}

	return true;
}

#define BLOCKMASK 0b10000 // A4
#define ADDRESSMASK 0b1110 //
#define BLOCK(address) ((address & BLOCKMASK) >> 3)
#define REG(address) ((address & ADDRESSMASK) >> 1)

static void* cfintf_decodereg(uint32_t address, bool write, bool sixteenbit) {

	if (!BLOCK(address)) { // Command block

		switch(REG(address)) {
			case 0x00:
			return &(tf.data);
			case 0x01:
			if(write) {
				return &(tf.error);
			}
			else {
				return &(tf.feature);
			}
			case 0x02:
			return &(tf.sectorcount);
			case 0x03:
			return &(tf.sectornumber);
			case 0x04:
			return &(tf.cylinderlow);
			case 0x05:
			return &(tf.cylinderhigh);
			case 0x06:
			return &(tf.drivehead);
			case 0x07:
			if(write) {
				log_println(LEVEL_DEBUG, TAG, "Command reg written.");
				return &(tf.command);
			}
			else {
				return &(tf.status);
			}

		}
	}
	else { // controlblock

		switch(REG(address)) {
			case 0x06:
			if(write) {
				return &(c.altstatus);
			}
			else {
				return &(c.devicecontrol);
			}
			case 0x07:
			return &(c.driveaddress);
		}
	}

	// shouldnt get here

	return NULL;
}

static uint8_t cfintf_read_byte(uint32_t address) {
	return *((uint8_t*) cfintf_decodereg(address, false, false));
}

static uint16_t cfintf_read_word(uint32_t address) {
	return *((uint16_t*) cfintf_decodereg(address, false, true));
}

static void cfintf_write_byte(uint32_t address, uint8_t value) {
	*((uint8_t*) cfintf_decodereg(address, true, false)) = value;
}

static void cfintf_write_word(uint32_t address, uint16_t value) {
	*((uint16_t*) cfintf_decodereg(address, true, true)) = value;
}

static void cfint_dispose() {
	if (munmap(image, size) > 0) {
		log_println(LEVEL_WARNING, TAG, "Failed to unmap image", errno);
	}
	close(fd);
	free((void*) idblock);
}

#define IDSERIAL "THEONLYONE          "
#define IDFWVER "0.01    "
#define IDMODEL "SIMULATEDCFCARD                         "

static uint8_t* cfint_createidblock() {

	uint8_t* block = malloc(512);
	memset(block, 0x00, 512);
	memcpy(block + (ATA_ID_SERIAL * 2), IDSERIAL, (ATA_ID_SERIAL_LEN * 2));
	memcpy(block + (ATA_ID_FIRMWAREVER * 2), IDFWVER, (ATA_ID_FIRMWAREVER_LEN * 2));
	memcpy(block + (ATA_ID_MODELNUMBER * 2), IDMODEL, (ATA_ID_MODELNUMBER_LEN * 2));

	//FIXME endian shit

	*((uint32_t*) (block + (ATA_ID_LBASECTORS * 2))) = size / 512;

	log_printhexblock(LEVEL_DEBUG, TAG, block, 512);

	return block;

}

static void cfint_init() {
	idblock = cfint_createidblock();
}

static void cfint_decodecommand() {

	switch (tf.command) {

		case ATA_IDENTIFYDRIVE:
			break;

	}

}

static void cfint_tick() {

	cfint_decodecommand();

}

const card compactflashinterfacecard = { "CF INTERFACE", cfint_init, cfint_dispose, cfint_tick, NULL, NULL,
		cfintf_read_byte, cfintf_read_word, NULL, cfintf_write_byte, cfintf_write_word, NULL };
