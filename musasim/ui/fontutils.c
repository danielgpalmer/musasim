/*
 * fontutils.c
 *
 *  Created on: 30 Sep 2012
 *      Author: daniel
 */

#include <stddef.h>
#include <string.h>
#include <stdlib.h>
#include <fontconfig/fontconfig.h>
#include "fontutils.h"
#include "../logging.h"

#define TAG "fontutils"

//fixme this is junk
char* fontutils_getmonospace() {

	FcInit();

	FcPattern* p = FcPatternBuild(0, FC_FONTFORMAT, FcTypeString, "TrueType",
	FC_FAMILY, FcTypeString, "monospace", (char *) 0);

	FcResult r;

	FcConfigSubstitute(NULL, p, FcMatchPattern);
	FcDefaultSubstitute(p);

	char* result = NULL;
	FcPattern* match = FcFontMatch(NULL, p, &r);

	FcChar8 *filename;
	if (FcPatternGetString(match, FC_FILE, 0, &filename) == FcResultMatch) {
		int len = strlen((char*) filename);
		result = malloc(len + 1);
		memcpy(result, filename, len);
	}

	FcPatternDestroy(p);
	FcPatternDestroy(match);

	FcFini();

	if (filename != NULL)
		log_println(LEVEL_INFO, TAG, "returning font %s", result);
	else
		log_println(LEVEL_INFO, TAG, "no suitable font was found!");

	return result;
}

