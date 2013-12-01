/*
 * osd.h
 *
 *  Created on: 16 Jun 2012
 *      Author: daniel
 */

#ifndef OSD_H_
#define OSD_H_

#include <stdbool.h>
#include <SDL.h>
bool osd_init(void);
void osd_update(double speed, double overhead);
void osd_visible(bool visible);
void osd_toggle(void);
void osd_render(SDL_Surface* screen);
void osd_dispose(void);

#endif /* OSD_H_ */
