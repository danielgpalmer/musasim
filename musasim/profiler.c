/*
 * profiler.c
 *
 *  Created on: Mar 31, 2012
 *      Author: daniel
 */

#include <sys/gmon_out.h>
#include "profiler.h"
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>

static bool enabled = false;
static FILE* file;
void profiler_init(const char* filepath) {

	struct gmon_hdr hdr = { .cookie = GMON_MAGIC, .version = { 0, 0, 0, GMON_VERSION } };
	memset(&hdr.spare, 0, sizeof(hdr.spare));

	file = fopen(filepath, "w");
	if (file == NULL) {
		printf("Failed to open %s!\n", filepath);
		return;
	}

	fwrite(&hdr, 1, sizeof(hdr), file);

	enabled = true;

}

void profiler_onpcmodified(uint32_t a1, uint32_t a2) {
	printf("Arc 0x%08x -> 0x%08x\n", a1, a2);
}

void profiler_onpcchange(uint32_t pc) {

	if (!enabled) {
		return;
	}

	uint32_t records = 1;
	uint8_t tag = 0x2;
	fwrite(&tag, 1, 1, file);
	fwrite(&records, 1, sizeof(uint32_t), file);
	uint32_t address = pc;
	uint32_t count = 1;
	fwrite(&count, 1, sizeof(uint32_t), file);
	fwrite(&address, 1, sizeof(uint32_t), file);

}

void profiler_flush() {

	if (!enabled) {
		return;
	}

	fclose(file);
	//GMON_Record_Tag tag = GMON_TAG_BB_COUNT;
	/*uint32_t records = 2048;
	 uint8_t tag = 0x2;
	 fwrite(&tag, 1, 1, file);
	 fwrite(&records, 1, sizeof(uint32_t), file);

	 for (int i = 0; i < records; i++) {
	 uint32_t address = i;
	 uint32_t count = i;
	 fwrite(&count, 1, sizeof(uint32_t), file);
	 fwrite(&address, 1, sizeof(uint32_t), file);
	 }*/

}
