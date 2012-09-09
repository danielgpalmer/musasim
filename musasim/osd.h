/*
 * osd.h
 *
 *  Created on: 16 Jun 2012
 *      Author: daniel
 */

#ifndef OSD_H_
#define OSD_H_

#include <stdbool.h>

void osd_init(void);
void osd_update(void);
void osd_visible(bool visible);
void osd_toggle(void);

#endif /* OSD_H_ */
