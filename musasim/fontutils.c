/*
 * fontutils.c
 *
 *  Created on: 30 Sep 2012
 *      Author: daniel
 */

#include <stddef.h>
#include <fontconfig/fontconfig.h>
#include "fontutils.h"

char* fontutils_getmonospace() {

	FcInit();

	FcPattern* p = FcPatternBuild(0, FC_FAMILY, FcTypeString, "Courier", (char *) 0);
	FcResult r;

	FcPattern* match = FcFontMatch(NULL, p, &r);

	FcPatternDestroy(p);
	FcPatternDestroy(match);

	FcFini();

	return "/usr/share/fonts/truetype/freefont/FreeMono.ttf";

}

