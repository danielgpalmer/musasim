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

bool elf_load(const char* path, uint8_t* dest, uint32_t destaddr, int maxlen) {
	FILE* file;
	Elf* e;

	GElf_Ehdr ehdr;

	if (elf_version(EV_CURRENT) == EV_NONE) {
		printf("libelf init failed\n");
		return false;
	}

	if (!(file = fopen(path, "r"))) {
		printf("Failed to open file %s\n", path);
		return false;
	}

	if ((e = elf_begin(file->_fileno, ELF_C_READ, NULL)) == NULL) {
		printf("Failed to open elf\n");
		return false;
	}

	if (gelf_getehdr(e, &ehdr) == NULL) {
		printf("Failed to get elf header!\n");
		return false;
	}

	if (ehdr.e_machine != EM_68K) {
		printf("Elf is not for M68K!\n");
		return false;
	}

	printf("This elf has %d prog headers\n", ehdr.e_phnum);
	for (int header = 0; header < ehdr.e_phnum; header++) {
		GElf_Phdr phdr;
		gelf_getphdr(e, header, &phdr);
		if (phdr.p_type == 0x1) {

			printf("Phy address 0x%08lx, Virt address 0x%08lx\n", phdr.p_paddr, phdr.p_vaddr);
			if (phdr.p_paddr >= destaddr + maxlen) {
				printf("Section is past the end of the area being loaded to, ignored\n");
				break;
			}

			if (fseek(file, phdr.p_offset, SEEK_SET) != 0) {
				printf("Failed to seek\n");
			}
			printf("Read %ld bytes\n", fread(dest + (phdr.p_paddr - destaddr), 1, phdr.p_filesz, file));

		}

	}

	return true;
}
