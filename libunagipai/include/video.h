/*
 * video.h
 *
 *  Created on: Jan 19, 2012
 *      Author: daniel
 */

#ifndef LIBUNAGIPAI_VIDEO_H_
#define LIBUNAGIPAI_VIDEO_H_

#define DATALOADERARGS (void* data, uint16_t* buff, int wanted)
typedef void (*dataloader)DATALOADERARGS;

void video_blitimage(int width, int height, int x, int y, void* data, dataloader);

#endif /* VIDEO_H_ */
