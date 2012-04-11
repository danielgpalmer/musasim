/*
 * compactflashinterfacecard.c
 *
 *  Created on: Dec 3, 2011
 *      Author: daniel
 */

#include <errno.h>
#include <ata_commands.h>
#include <ata_idoffsets.h>
#include <ata_registermasks.h>
#include <fcntl.h>
#include <stdint.h>
#include <stdbool.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>

#include "compactflashinterfacecard.h"
#include "../../logging.h"

static const char TAG[] = "cfint";

typedef struct {
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
	uint8_t devicecontrol;
	uint8_t driveaddress;
} control;

static taskfile tf;
static control c;

static int fd;
static uint8_t* idblock;
static uint8_t* image;
static size_t size;

static int busycounter = 0;
static int transfercount = 0;
static int transfercounter = 0;

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

#define ADDRESSMASK 0b1110 // DA2, DA1, DA0
#define BLOCK(address) ((address & BLOCKMASK) >> 3)
#define REG(address) ((address & ADDRESSMASK) >> 1)

static bool commandregdirty = false;

static void* cfintf_decodereg(uint32_t address, bool write, bool sixteenbit) {
	if (!BLOCK(address)) { // Command block
		switch (REG(address)) {
			case 0x00:

				if (!sixteenbit) {
					log_println(LEVEL_DEBUG, TAG, "reads/writes to the data reg should be 16bits");
				}

				log_println(LEVEL_DEBUG, TAG, "read from/write to data reg -- counter %d", transfercounter);
				transfercounter++;
				if (transfercounter == transfercount) {
					tf.status &= ~ATA_STATUS_DRQ;
				}
				return &(idblock[0]);
			case 0x01:
				if (write) {
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
				if (write) {
					log_println(LEVEL_DEBUG, TAG, "Command reg written.");
					commandregdirty = true;
					return &(tf.command);
				}
				else {
					return &(tf.status);
				}

		}
	}
	else { // controlblock
		switch (REG(address)) {
			case ALTSTATUSOFFSET:
				if (write) {
					return &(c.devicecontrol);
				}
				else {
					return &(tf.status);
				}
			case 0x07:
				return &(c.driveaddress);
		}
	}

	// shouldnt get here

	return NULL;
}

static void cfint_decodecommand() {

	if (commandregdirty) {
		log_println(LEVEL_DEBUG, TAG, "decoding command");
		switch (tf.command) {
			case ATA_IDENTIFYDRIVE:
				log_println(LEVEL_DEBUG, TAG, "identify!");
				tf.status |= ATA_STATUS_BSY;
				busycounter = 10;
				transfercounter = 0;
				transfercount = 256;
				break;
		}
		commandregdirty = false;
	}
}

static uint8_t cfintf_read_byte(uint32_t address) {

	uint8_t* reg = ((uint8_t*) cfintf_decodereg(address, false, false));

	if (reg == NULL) {
		return 0;
	}

	return *reg;
}

static uint16_t cfintf_read_word(uint32_t address) {

	uint16_t* reg = ((uint16_t*) cfintf_decodereg(address, false, true));

	if (reg == NULL) {
		return 0;
	}

	return *reg;
}

static void cfintf_write_byte(uint32_t address, uint8_t value) {
	uint8_t* reg = cfintf_decodereg(address, true, false);
	if (reg != NULL) {
		*reg = value;
		cfint_decodecommand(); // only a byte write has the potential to cause the command register to be dirty
	}
}

static void cfintf_write_word(uint32_t address, uint16_t value) {
	uint16_t* reg = (uint16_t*) cfintf_decodereg(address, true, true);
	if (reg != NULL) {
		*reg = value;
	}
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

static void cfint_tick() {
	if (busycounter > 0) {
		busycounter--;
		if (busycounter == 0) {
			log_println(LEVEL_DEBUG, TAG, "transfer ready");
			tf.status &= ~ATA_STATUS_BSY;
			tf.status |= ATA_STATUS_DRQ;
		}
	}
}

const card compactflashinterfacecard = { "CF INTERFACE", cfint_init, cfint_dispose, cfint_tick, NULL, NULL,
		cfintf_read_byte, cfintf_read_word, NULL, cfintf_write_byte, cfintf_write_word, NULL };
