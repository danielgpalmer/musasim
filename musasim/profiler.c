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
#include <glib.h>

static GHashTable* callarcs;
static GHashTable* blockcounts;

typedef struct {
	uint32_t from;
	uint32_t to;
	uint32_t count;
} callarc_t;

typedef struct {
	uint32_t address;
	uint32_t count;
} blockcount_t;

static bool enabled = false;
static FILE* file;

static uint32_t* pointertobeuint(uint32_t uint) {
#if G_BYTE_ORDER == G_LITTLE_ENDIAN
	static uint32_t beuint;
	beuint = GUINT32_TO_BE(uint);
	return &beuint;
#else
	return &uint;
#endif
}

void profiler_init(const char* filepath) {

	struct gmon_hdr hdr = { .cookie = GMON_MAGIC, .version = { 0, 0, 0, GMON_VERSION } };
	memset(&hdr.spare, 0, sizeof(hdr.spare));

	file = fopen(filepath, "w");
	if (file == NULL) {
		printf("Failed to open %s!\n", filepath);
		return;
	}

	fwrite(&hdr, 1, sizeof(hdr), file);

	callarcs = g_hash_table_new(g_str_hash, g_str_equal);
	blockcounts = g_hash_table_new(g_str_hash, g_str_equal);

	enabled = true;

}

void profiler_onpcmodified(uint32_t a1, uint32_t a2) {
	char key[17];
	snprintf(key, 17, "%08x%08x", a1, a2);
	if (!g_hash_table_contains(callarcs, &key)) {
		callarc_t* callarc = malloc(sizeof(callarc_t));
		callarc->from = a1;
		callarc->to = a2;
		callarc->count = 1;
		g_hash_table_insert(callarcs, key, callarc);
	}
	else {
		callarc_t* callarc = g_hash_table_lookup(callarcs, key);
		if (callarc->from != a1 && callarc->to != a2) {
			printf("um...\n");
		}
		callarc->count += 1;
	}
}

void profiler_onpcchange(uint32_t pc) {

	if (!enabled) {
		return;
	}

	char key[9];
	snprintf(key, 9, "%08x", pc);
	if (!g_hash_table_contains(blockcounts, &key)) {
		blockcount_t* blockcount = malloc(sizeof(blockcount_t));
		blockcount->address = pc;
		blockcount->count = 1;
		g_hash_table_insert(blockcounts, key, blockcount);
	}
	else {
		blockcount_t* blockcount = g_hash_table_lookup(blockcounts, key);
		if (blockcount->address != pc) {
			printf("um...\n");
		}
		blockcount->count += 1;
	}

}

void profiler_flush() {

	if (!enabled) {
		return;
	}

	GHashTableIter iter;
	gpointer key, value;

	g_hash_table_iter_init(&iter, callarcs);
	while (g_hash_table_iter_next(&iter, &key, &value)) {
		callarc_t* callarc = value;
		printf("0x%08x -> 0x%08x, %d\n", callarc->from, callarc->to, callarc->count);
		uint8_t tag = GMON_TAG_CG_ARC;
		fwrite(&tag, 1, 1, file);
		fwrite(pointertobeuint(callarc->from), 1, sizeof(uint32_t), file);
		fwrite(pointertobeuint(callarc->to), 1, sizeof(uint32_t), file);
		fwrite(pointertobeuint(callarc->count), 1, sizeof(uint32_t), file);
	}

//GMON_Record_Tag tag = GMON_TAG_BB_COUNT;
	uint32_t records = g_hash_table_size(blockcounts);
	printf("records - %d\n", records);
	uint8_t tag = GMON_TAG_BB_COUNT;
	fwrite(&tag, 1, 1, file);
	fwrite(&records, 1, sizeof(uint32_t), file); // mmm it wants this in the host endian??!

	g_hash_table_iter_init(&iter, blockcounts);
	while (g_hash_table_iter_next(&iter, &key, &value)) {
		blockcount_t* blockcount = value;
		fwrite(pointertobeuint(blockcount->address), 1, sizeof(uint32_t), file);
		fwrite(pointertobeuint(blockcount->count), 1, sizeof(uint32_t), file);
	}

	g_hash_table_destroy(callarcs);
	g_hash_table_destroy(blockcounts);

	fclose(file);
}
