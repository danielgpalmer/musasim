/*
 * sdlwrapper.c
 *
 *  Created on: 8 Dec 2012
 *      Author: daniel
 */

#include <SDL.h>
#include <SDL_gfxPrimitives.h>

void sdlwrapper_plot(SDL_Surface* dst, int x, int y, uint32_t colour) {
	pixelColor(dst, x, y, colour);
}

void sdlwrapper_drawline(SDL_Surface* dst, int x1, int y1, int x2, int y2, int width, uint32_t colour) {
	lineColor(dst, x1, y1, x2, y2, colour);
}
