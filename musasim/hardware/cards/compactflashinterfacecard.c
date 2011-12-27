/*
 * compactflashinterfacecard.c
 *
 *  Created on: Dec 3, 2011
 *      Author: daniel
 */

#include "compactflashinterfacecard.h"

#include <errno.h>
#include <fcntl.h>
#include <stdint.h>
#include <stdbool.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <unistd.h>
#include "../../logging.h"

static const char TAG[] = "cfint";

static uint16_t dataport;
static uint16_t statusport;

static uint16_t* registers[] = { &dataport, NULL, NULL, NULL, NULL, NULL, &statusport };

static int fd;
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

#define ADDRESSMASK 0b111

#define REG(address) (address & ADDRESSMASK)

uint8_t cfintf_read_byte(uint32_t address) {
	return *(registers[REG(address)]);
}

uint16_t cfintf_read_word(uint32_t address) {
	int reg = REG(address);
	if (reg != 0) {
		return 0;
	}
	return *(registers[REG(address)]);
}

void cfintf_write_byte(uint32_t address, uint8_t value) {
	*(registers[REG(address)]) = value;
}

void cfintf_write_word(uint32_t address, uint16_t value) {
	int reg = REG(address);
	if (reg != 0) {
		return;
	}

}

void cfint_dispose() {
	if (munmap(image, size) > 0) {
		log_println(LEVEL_WARNING, TAG, "Failed to unmap image", errno);
	}
	close(fd);
}

const card compactflashinterfacecard = { "CF INTERFACE", NULL, cfint_dispose, NULL, NULL, NULL, cfintf_read_byte,
		cfintf_read_word, NULL, cfintf_write_byte, cfintf_write_word, NULL };
