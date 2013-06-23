#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <libunagipai/input_registers.h>
#include <libunagipai/input_registermasks.h>
#include <libunagipai/video.h>
#include <libunagipai/video_registers.h>
#include <libunagipai/rect.h>
#include <libunagipai/utils.h>

#define ONCOLOUR 0xf800
#define OFFCOLOUR 0xffff

#define RECTSIZE 20
#define STARTRECTWIDTH (RECTSIZE * 2)
#define PLAYFIELDMIDDLE (VIDEO_WIDTH/2)
#define QUARTERPLAYFIELD ((VIDEO_WIDTH/4) - RECTSIZE)
#define UPBUTTONTOP 20
#define DOWNBUTTONTOP (UPBUTTONTOP + (RECTSIZE * 2) + (PAD * 2))
#define LEFTRIGHTTOP (UPBUTTONTOP + RECTSIZE + PAD)
#define LETTERBUTTONSROW 120
#define LEFTLEFT (QUARTERPLAYFIELD - RECTSIZE - PAD)
#define RIGHTLEFT (QUARTERPLAYFIELD + RECTSIZE + PAD)
#define CBUTTON 150
#define BBUTTON (CBUTTON - RECTSIZE - PAD)
#define ABUTTON (BBUTTON - RECTSIZE - PAD)
#define STARTBUTTON (ABUTTON - STARTRECTWIDTH  - PAD)
#define PAD 10

static rect p0[] = { { CBUTTON, LETTERBUTTONSROW, RECTSIZE, RECTSIZE }, // C
		{ BBUTTON, LETTERBUTTONSROW, RECTSIZE, RECTSIZE }, // B
		{ ABUTTON, LETTERBUTTONSROW, RECTSIZE, RECTSIZE }, // A
		{ STARTBUTTON, LETTERBUTTONSROW, STARTRECTWIDTH, RECTSIZE }, // START
		{ RIGHTLEFT, LEFTRIGHTTOP, RECTSIZE, RECTSIZE }, // RIGHT
		{ LEFTLEFT, LEFTRIGHTTOP, RECTSIZE, RECTSIZE }, // LEFT
		{ QUARTERPLAYFIELD, DOWNBUTTONTOP, RECTSIZE, RECTSIZE }, // DOWN
		{ QUARTERPLAYFIELD, UPBUTTONTOP, RECTSIZE, RECTSIZE } // UP
};

static rect p1[] = { { PLAYFIELDMIDDLE + CBUTTON, LETTERBUTTONSROW, RECTSIZE, RECTSIZE }, // C
		{ PLAYFIELDMIDDLE + BBUTTON, LETTERBUTTONSROW, RECTSIZE, RECTSIZE }, // B
		{ PLAYFIELDMIDDLE + ABUTTON, LETTERBUTTONSROW, RECTSIZE, RECTSIZE }, // A
		{ PLAYFIELDMIDDLE + STARTBUTTON, LETTERBUTTONSROW, STARTRECTWIDTH, RECTSIZE }, // START
		{ PLAYFIELDMIDDLE + RIGHTLEFT, LEFTRIGHTTOP, RECTSIZE, RECTSIZE }, // RIGHT
		{ PLAYFIELDMIDDLE + LEFTLEFT, LEFTRIGHTTOP, RECTSIZE, RECTSIZE }, // LEFT
		{ PLAYFIELDMIDDLE + QUARTERPLAYFIELD, DOWNBUTTONTOP, RECTSIZE, RECTSIZE }, // DOWN
		{ PLAYFIELDMIDDLE + QUARTERPLAYFIELD, UPBUTTONTOP, RECTSIZE, RECTSIZE } // UP
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
