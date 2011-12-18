void video_init();
void video_tick();
void video_quit();

uint8_t video_read_byte(uint32_t address);
uint16_t video_read_word(uint32_t address);

void video_write_byte(uint32_t address, uint8_t data);
void video_write_word(uint32_t address, uint16_t data);

bool registerwritecheck();
void dumpregs();

#define VIDEO_PIXELFORMAT 16
#define VIDEO_PIXELSIZE (VIDEO_PIXELFORMAT/8)
#define VIDEO_WIDTH 480
#define VIDEO_HEIGHT 272
#define VIDEO_MEMORYEND (VIDEO_WIDTH * VIDEO_HEIGHT) * VIDEO_PIXELSIZE

#define VIDEO_REG_FLAGS 0
#define VIDEO_REG_CONFIG 1

uint32_t video_fillbits(uint32_t value) {
	value--;
	value |= value >> 1;
	value |= value >> 2;
	value |= value >> 4;
	value |= value >> 8;
	value |= value >> 16;
	value++;
	return value;
}

#include "card.h"

card videocard;

