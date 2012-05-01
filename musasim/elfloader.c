/*
 * elfloader.c
 *
 *  Created on: Mar 30, 2012
 *      Author: daniel
 */

#include <libelf.h>
#include <gelf.h>
#include <stdint.h>
#include <fcntl.h>
#include "elfloader.h"
#include <stdio.h>
#include "logging.h"

static const char TAG[] = "elfloader";

bool elf_load(const char* path, uint8_t* dest, uint32_t destaddr, int maxlen) {
	FILE* file;
	Elf* e;

	GElf_Ehdr ehdr;

	if (elf_version(EV_CURRENT) == EV_NONE) {
		log_println(LEVEL_WARNING, TAG, "libelf init failed");
		return false;
	}

	if (!(file = fopen(path, "r"))) {
		log_println(LEVEL_WARNING, TAG, "Failed to open file %s", path);
		return false;
	}

	if ((e = elf_begin(file->_fileno, ELF_C_READ, NULL)) == NULL) {
		log_println(LEVEL_WARNING, TAG, "libelf failed to parse %s", path);
		return false;
	}

	if (gelf_getehdr(e, &ehdr) == NULL) {
		log_println(LEVEL_WARNING, TAG, "Failed to get elf header!");
		return false;
	}

	if (ehdr.e_machine != EM_68K) {
		log_println(LEVEL_WARNING, TAG, "Elf is not for M68K!");
		return false;
	}

	if (ehdr.e_phnum == 0) {
		log_println(LEVEL_WARNING, TAG, "this elf file has no prog headers!! not an elf file?\n");
		return false;
	}

	log_println(LEVEL_INFO, TAG, "This elf has %d prog headers", ehdr.e_phnum);

	for (int header = 0; header < ehdr.e_phnum; header++) {
		GElf_Phdr phdr;
		gelf_getphdr(e, header, &phdr);
		if (phdr.p_type == 0x1) {

			log_println(LEVEL_INFO, TAG, "Phy address 0x%08lx, Virt address 0x%08lx\n", phdr.p_paddr, phdr.p_vaddr);
			if (phdr.p_paddr >= destaddr + maxlen) {
				log_println(LEVEL_INFO, TAG, "Section is past the end of the area being loaded to, ignored\n");
				continue;
			}

			if (fseek(file, phdr.p_offset, SEEK_SET) != 0) {
				printf("Failed to seek\n");
				return false;
			}
			fread(dest + (phdr.p_paddr - destaddr), 1, phdr.p_filesz, file);

		}

	}

	return true;
}
