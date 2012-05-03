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

#include <glib.h>
#include "compactflashinterfacecard.h"
#include "../../logging.h"

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
	uint8_t devicecontrol;
	uint8_t driveaddress;
} control;

static taskfile tf;
static control c;

static int fd;
static uint8_t* idblock;
static uint16_t* image;
static size_t size;

static int busycounter = 0;
static int transfercount = 0;
static int transfercounter = 0;

#define SECTORSIZE 512

static bool loaded = false;

static uint32_t lbasectors = 0;

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

	*((uint32_t*) (block + (ATA_ID_LBASECTORS * 2))) = lbasectors;

	log_printhexblock(LEVEL_INFO, TAG, block, 512);

	return block;

}

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
	lbasectors = size / SECTORSIZE;

	log_println(LEVEL_INFO, TAG, "Image is %d bytes", (int) size);

	if ((image = mmap(NULL, size, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0)) == MAP_FAILED) {
		log_println(LEVEL_WARNING, TAG, "Failed to map image, %d", errno);
		return false;
	}

	idblock = cfint_createidblock();
	loaded = true;
	return true;
}

#define ADDRESSMASK 0b1110 // DA2, DA1, DA0
#define BLOCK(address) ((address & BLOCKMASK) >> 3)
#define REG(address) ((address & ADDRESSMASK) >> 1)

static bool commandregdirty = false;

#define LBAADDRESS (tf.sectornumber | (tf.cylinderlow << 8) | (tf.cylinderhigh << 16))

static void* cfintf_decodereg(uint32_t address, bool write, bool sixteenbit) {
	if (!BLOCK(address)) { // Command block
		switch (REG(address)) {
			case 0x00:

				if (!sixteenbit) {
					log_println(LEVEL_DEBUG, TAG, "reads/writes to the data reg should be 16bits");
					return NULL;
				}

				//log_println(LEVEL_DEBUG, TAG, "read from/write to data reg -- counter %d", transfercounter);

				if (write) {
				}
				else {
					switch (tf.command) {
						case ATA_IDENTIFYDRIVE:
							tf.data = (idblock[(transfercounter * 2) + 1] << 8) | idblock[(transfercounter * 2)];
							break;
						case ATA_READBUFFER: //TODO calculate the actual block that's being read
							tf.data = GUINT16_FROM_BE(image[(LBAADDRESS * 256) + transfercounter]);
							break;
					}
				}

				transfercounter++;
				if (transfercounter == transfercount) {
					tf.status &= ~ATA_STATUS_DRQ;
				}
				return &(tf.data);
			case ERRORFEATURE:
				if (write) {
					return &(tf.feature);
				}
				else {
					return &(tf.error);
				}
			case SECTORCOUNT:
				return &(tf.sectorcount);
			case SECTORNUMBER:
				return &(tf.sectornumber);
			case CYLINDERLOW:
				return &(tf.cylinderlow);
			case CYLINDERHIGH:
				return &(tf.cylinderhigh);
			case DRIVEHEAD:
				return &(tf.drivehead);
			case COMMANDSTATUS:
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
				if (write) {
					return NULL;
				}
				else {
					return &(c.driveaddress);
				}
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
			case ATA_READBUFFER:
				log_println(LEVEL_DEBUG, TAG, "read!");
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

	if (!loaded) {
		return 0x0;
	}

	uint8_t* reg = ((uint8_t*) cfintf_decodereg(address, false, false));

	if (reg == NULL) {
		return 0;
	}

	return *reg;
}

static uint16_t cfintf_read_word(uint32_t address) {

	if (!loaded) {
		return 0x0;
	}

	uint16_t* reg = ((uint16_t*) cfintf_decodereg(address, false, true));

	if (reg == NULL) {
		return 0;
	}

	return *reg;
}

static void cfintf_write_byte(uint32_t address, uint8_t value) {

	if (!loaded) {
		return;
	}

	uint8_t* reg = cfintf_decodereg(address, true, false);
	if (reg != NULL) {
		*reg = value;
		cfint_decodecommand(); // only a byte write has the potential to cause the command register to be dirty
	}
}

static void cfintf_write_word(uint32_t address, uint16_t value) {
	if (!loaded) {
		return;
	}

	uint16_t* reg = (uint16_t*) cfintf_decodereg(address, true, true);
	if (reg != NULL) {
		*reg = value;
	}
}

static void cfint_dispose() {
	loaded = false;
	if (munmap(image, size) > 0) {
		log_println(LEVEL_WARNING, TAG, "Failed to unmap image -- error %d", errno);
	}
	close(fd);
	free((void*) idblock);
}

static void cfint_init() {
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

static bool cfint_validaddress(uint32_t address) {
	return true;
}

const card compactflashinterfacecard = { "CF INTERFACE", cfint_init, cfint_dispose, NULL, cfint_tick, NULL, NULL,
		cfint_validaddress, cfintf_read_byte, cfintf_read_word, NULL, cfintf_write_byte, cfintf_write_word, NULL };
