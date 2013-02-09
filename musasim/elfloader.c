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
#include <stdio.h>
#include <inttypes.h>
#include "logging.h"
#include "elfloader.h"

static const char TAG[] = "elfloader";

bool elf_load(const char* path, uint8_t* dest, uint32_t destaddr, int maxlen) {

	FILE* file = NULL;
	Elf* e = NULL;
	GElf_Ehdr ehdr;

	bool ret = false;

	if (elf_version(EV_CURRENT) == EV_NONE) {
		log_println(LEVEL_WARNING, TAG, "libelf init failed");
		goto exit;
	}

	if (!(file = fopen(path, "r"))) {
		log_println(LEVEL_WARNING, TAG, "Failed to open file %s", path);
		goto exit;
	}

	if ((e = elf_begin(file->_fileno, ELF_C_READ, NULL )) == NULL ) {
		log_println(LEVEL_WARNING, TAG, "libelf failed to parse %s", path);
		goto exit;
	}

	if (gelf_getehdr(e, &ehdr) == NULL ) {
		log_println(LEVEL_WARNING, TAG, "Failed to get elf header!");
		goto exit;
	}

	if (ehdr.e_machine != EM_68K) {
		log_println(LEVEL_WARNING, TAG, "Elf is not for M68K!");
		goto exit;
	}

	if (ehdr.e_phnum == 0) {
		log_println(LEVEL_WARNING, TAG, "this elf file has no prog headers!! not an elf file?");
		goto exit;
	}

	log_println(LEVEL_INFO, TAG, "This elf has %d prog headers", ehdr.e_phnum);

	for (int header = 0; header < ehdr.e_phnum; header++) {
		GElf_Phdr phdr;
		gelf_getphdr(e, header, &phdr);
		if (phdr.p_type == 0x1) {

			log_println(LEVEL_INFO, TAG, "Phy address 0x%08"PRIx64", Virt address 0x%08"PRIx64, phdr.p_paddr,
					phdr.p_vaddr);
			if (phdr.p_paddr >= destaddr + maxlen) {
				log_println(LEVEL_INFO, TAG, "Section is past the end of the area being loaded to, ignored");
				continue;
			}

			if (fseek(file, phdr.p_offset, SEEK_SET) != 0) {
				log_println(LEVEL_WARNING, TAG, "Failed to seek in ELF\n");
				break;
			}
			else {
				fread(dest + (phdr.p_paddr - destaddr), 1, phdr.p_filesz, file);
				ret = true;
			}
		}

	}

	exit:

	if (file != NULL ) {
		fclose(file);
	}
	if (e != NULL ) {
		elf_end(e);
	}

	return ret;
}
