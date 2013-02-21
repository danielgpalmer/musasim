/*
 * fontutils.c
 *
 *  Created on: 30 Sep 2012
 *      Author: daniel
 */

#include <stddef.h>
#include <fontconfig/fontconfig.h>
#include "fontutils.h"

//fixme this is junk
char* fontutils_getmonospace() {

	FcInit();

	FcPattern* p = FcPatternBuild(0, FC_FONTFORMAT, FcTypeString, "TrueType", FC_FAMILY, FcTypeString, "monospace",
			(char *) 0);

	FcResult r;

	FcConfigSubstitute(NULL, p, FcMatchPattern);
	FcDefaultSubstitute(p);

	char* result = NULL;
	FcPattern* match = FcFontMatch(NULL, p, &r);

	FcChar8 *filename;
	FcPatternGetString(match, FC_FILE, 0, &filename);

	FcPatternDestroy(p);
	FcPatternDestroy(match);

	result = (char*) filename;
	FcFini();

	return result;
}

