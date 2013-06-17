#include <stdbool.h>
#include <stdint.h>
#include <libunagipai/input_registers.h>
#include <libunagipai/input_registermasks.h>
#include <libunagipai/video.h>
#include <libunagipai/rect.h>
#include <libunagipai/utils.h>

#define ONCOLOUR 0xf800
#define OFFCOLOUR 0xffff

#define RECTSIZE 20

static rect p0[] = { { 100, 100, RECTSIZE, RECTSIZE }, // C
		{ 65, 100, RECTSIZE, RECTSIZE }, // B
		{ 40, 100, RECTSIZE, RECTSIZE }, // A
		{ 50, 40, RECTSIZE, RECTSIZE }, // START
		{ 75, 40, RECTSIZE, RECTSIZE }, // RIGHT
		{ 90, 40, RECTSIZE, RECTSIZE }, // LEFT
		{ 110, 40, RECTSIZE, RECTSIZE }, // DOWN
		{ 135, 40, RECTSIZE, RECTSIZE } // UP
};

static rect p1[] = { { 10, 10, 50, 50 }, // C
		{ 10, 10, 50, 50 }, // B
		{ 10, 10, 50, 50 }, // A
		{ 10, 10, 50, 50 }, // START
		{ 10, 10, 50, 50 }, // RIGHT
		{ 10, 10, 50, 50 }, // LEFT
		{ 10, 10, 50, 50 }, // DOWN
		{ 10, 10, 50, 50 } // UP
};

static uint8_t port0 = 0xff;
static uint8_t port1 = 0xff;

static void update() {
	video_begin();

	for (int r = 0; r < SIZEOFARRAY(p0); r++) {
		rect_drawrect(&p0[r], (((port0 >> r) & 1) == 0) ? ONCOLOUR : OFFCOLOUR);
	}

	for (int r = 0; r < SIZEOFARRAY(p1); r++) {
		rect_drawrect(&p1[r], (((port1 >> r) & 1) == 0) ? ONCOLOUR : OFFCOLOUR);
	}

	video_commit();
}

void main() {

	printf("Starting input test\n");
	video_waitforvblank();
	video_setconfig(false, false);

	update();
	video_waitforvblank();
	video_flip();

	while (1) {
		uint8_t newport0 = input_port0;
		uint8_t newport1 = input_port1;
		if ((port0 != newport0) || (port1 != newport1)) {
			port0 = newport0;
			port1 = newport1;
			update();
			video_waitforvblank();
			video_flip();
		}
		else {
			video_waitforvblank();
		}
	}
}
