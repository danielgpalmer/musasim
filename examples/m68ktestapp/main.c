/*
 * main.c
 *
 *  Created on: Jul 17, 2011
 *      Author: daniel
 */

#include <libunagipai/machine.h>
#include <libunagipai/pff.h>
#include <libunagipai/video_registers.h>
#include <libunagipai/video_registermasks.h>
#include <libunagipai/dma_registers.h>
#include <libunagipai/dma_registermasks.h>
#include <libunagipai/input_registers.h>
#include <libunagipai/sound_registers.h>
#include <libunagipai/image.h>
#include <libunagipai/sprite.h>
#include <libunagipai/math.h>
#include <libunagipai/utils.h>
#include <libunagipai/video.h>

#include "main.h"

#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <inttypes.h>

#include "fontrom.h"

#include <libunagipai/video.h>

#include "blip.c" // cant be arsed with linking right now
void interrupthandler() {
}

void sound_handler() {

}

typedef struct {
	sprite* sprite;
	int xinc;
	int yinc;
	int speedx;
	int speedy;
} ball;

static image ballimage;
static image pai;
static ball ball1, ball2;

static void updateball(ball* b, uint16_t thisframe, uint16_t lastframe) {

	//for (int i = 0; i < thisframe - lastframe; i++) {

	b->sprite->x += (b->speedx * b->xinc);

	if (b->sprite->x >= VIDEO_WIDTH - b->sprite->image->width - 1 || b->sprite->x <= 0) {
		b->sprite->x = b->xinc > 0 ? VIDEO_WIDTH - 1 - b->sprite->image->width : 0;
		b->xinc = -b->xinc;
	}

	b->sprite->y += (b->speedy * b->yinc);

	if (b->sprite->y >= VIDEO_HEIGHT - b->sprite->image->height - 1 || b->sprite->y <= 0) {
		b->sprite->y = b->yinc > 0 ? VIDEO_HEIGHT - 1 - b->sprite->image->height : 0;
		b->yinc = -b->yinc;
	}
	//}

}

static void newball(ball* b, int x, int y, image* image) {

	b->sprite = malloc(sizeof(sprite));
	b->sprite->image = &ballimage;
	b->sprite->x = x;
	b->sprite->y = y;
	b->xinc = 1;
	b->yinc = 1;
	b->speedx = CLAMP(1, 3, input_rng);
	b->speedy = CLAMP(1, 3, input_rng);
}

static void ballrelect(ball* b1, ball* b2, vector* vect) {

	if (vect->x2 > vect->x1) {
		b1->xinc = -1;
		b2->xinc = 1;
	}
	else {
		b2->xinc = -1;
		b1->xinc = 1;
	}

	if (vect->y2 > vect->y1) {
		b1->yinc = -1;
		b2->yinc = 1;
	}
	else {
		b2->yinc = -1;
		b1->yinc = 1;
	}

}

static vector vect;
static void ballcollision(ball* b1, ball* b2) {
	vect.x1 = b1->sprite->x + (b1->sprite->image->width / 2);
	vect.y1 = b1->sprite->y + (b1->sprite->image->height / 2);
	vect.x2 = b2->sprite->x + (b2->sprite->image->width / 2);
	vect.y2 = b2->sprite->y + (b2->sprite->image->height / 2);
	if (sprite_checkoverlap(b1->sprite, b2->sprite)) {
		ballrelect(b1, b2, &vect);
	}
}

static volatile bool fbready = false;

void vblank_handler() {
	//printf("vblank\n");
	if (fbready) {
		video_flip();
		fbready = false;
	}
}

void uart_handler() {
}

void initvideo() {
	//video_begin();
	//video_clear(0xFFFF);
	//video_commit();
	//video_waitforvblank();
	//video_flip();

	//video_begin();
	//video_clear(0xFFFF);
	//video_commit();
	video_waitforvblank();
	video_setconfig(true, false);
	//video_flip();
}

void fatimageloader DATALOADERARGS {
	uint16_t len;
	pf_read(buff, wanted, &len);

}

int main(void) {

	//*uart_chan0_interruptenable |= INTERRUPTENABLE_ERBFI;

	//uint16_t* blip = _binary_blip_start;

	//for (int i = 0; i < sizeof(_binary_blip_start) / 2; i++) {
	//	*(sound_bank_0 + i) = *blip++;
	//}
	//*sound_channel_0_samplelength = sizeof(_binary_blip_start);
	//*sound_channel_master_config = 0xFFFF;
	//*sound_channel_master_volume = 0xFF99;
	//*sound_channel_0_volume = 0xFF22;
	//*sound_channel_0_config = 0xF9FF;

	FATFS fs;
	FRESULT result;
	result = pf_mount(&fs);
	util_printffresult(result);
	util_printfat(&fs);
	result = pf_open("test.txt");
	util_printffresult(result);
	char buf[64];
	uint16_t len;
	pf_read(buf, 63, &len);

	printf("read from file: %s\n", buf);

	image_loadimagefromfile(&fs, &pai, "pai.bz", true);
	image_loadimagefromfile(&fs, &ballimage, "ball.be", false);

	newball(&ball1, 0, 0, &ballimage);
	newball(&ball2, 50, 0, &ballimage);

	initvideo();

	machine_setinterruptmask(0);

	while (1) {

		if (fbready)
			continue;

		static uint16_t lastframe = 0;
		static uint16_t thisframe;

		uint16_t vidflags = video_register_flags;
		uint8_t port0 = input_port0;

		thisframe = video_register_frame;

		updateball(&ball1, thisframe, lastframe);
		updateball(&ball2, thisframe, lastframe);
		ballcollision(&ball1, &ball2);

		video_begin();
		video_clear(0xFFFF);
		video_blitimage_nocopy(pai.width, pai.height, 30, 30, pai.data);
		sprite_draw(ball1.sprite);
		sprite_draw(ball2.sprite);
		video_drawline(&vect);
		video_gputs("Hello World!", _binary_fontrom_start, 1, 1);

		lastframe = thisframe;
		video_commit();
		//video_flip();
		fbready = true;

	}

	printf("Shouldn't have got here!");

	while (1) {

	}

	return 0;

}
