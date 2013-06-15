#include <stdbool.h>
#include <stdint.h>
#include <libunagipai/input_registers.h>
#include <libunagipai/input_registermasks.h>
#include <libunagipai/video.h>
#include <libunagipai/rect.h>

static rect p0up = { 10, 10, 50, 50 };
//static rect p0down, p0left, p0right;

static void update() {
	video_begin();
	rect_drawrect(&p0up, 0xffff);
	video_commit();
	video_waitforvblank();
	video_flip();
}

int main() {
	video_waitforvblank();
	video_setconfig(false, false);

	while (1) {
		update();
	}
}
