/*
 * sdlwrapper.h
 *
 *  Created on: 8 Dec 2012
 *      Author: daniel
 */

#ifndef SDLWRAPPER_H_
#define SDLWRAPPER_H_

void sdlwrapper_plot(SDL_Surface* dst, int x, int y, uint32_t colour);
void sdlwrapper_drawline(SDL_Surface* dst, int x1, int y1, int x2, int y2, int width, uint32_t colour);

#endif /* SDLWRAPPER_H_ */
