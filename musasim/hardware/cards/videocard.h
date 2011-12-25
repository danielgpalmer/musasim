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
#define HBLANKPERIOD 20
#define VBLANKPERIOD 28
#define VIDEO_MEMORYEND (VIDEO_WIDTH * VIDEO_HEIGHT) * VIDEO_PIXELSIZE
#define VIDEO_REFRESHRATE 60
#define VIDEO_ACTIVEPIXELS (VIDEO_WIDTH * VIDEO_HEIGHT)
#define VIDEO_TOTALPIXELS ((VIDEO_WIDTH + HBLANKPERIOD) * (VIDEO_HEIGHT + VBLANKPERIOD))
#define VIDEO_PIXELSPERSECOND (VIDEO_TOTALPIXELS * VIDEO_REFRESHRATE)

#define VIDEO_REG_FLAGS 0
#define VIDEO_REG_CONFIG 2
#define VIDEO_REG_FRAME 4

#include "card.h"

const card videocard;

