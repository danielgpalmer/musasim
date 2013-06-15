/**
 *   \file video_registers.h
 *   \author genheader
 *   \brief Video register defines
 */
#ifndef LIBUNAGIPAI_VIDEOREGISTERS_H_
#define LIBUNAGIPAI_VIDEOREGISTERS_H_


#include <stdint.h>


#define VIDEO_WIDTH 0x1e0
#define VIDEO_HEIGHT 0x110
#define VIDEO_PLAYFIELDWIDTH 0x200
#define VIDEO_PLAYFIELDHEIGHT 0x200
#define VIDEO_PIXELSIZE 0x2
#define VIDEO_LINESIZE 0x400
#define video_register_flags (*(volatile uint16_t*) 0x200000)
#define video_register_config (*(volatile uint16_t*) 0x200002)
#define video_register_pixel (*(volatile uint16_t*) 0x200004)
#define video_register_line (*(volatile uint16_t*) 0x200006)
#define video_register_frame (*(volatile uint16_t*) 0x200008)
#define video_register_posx (*(volatile uint16_t*) 0x20000a)
#define video_register_posy (*(volatile uint16_t*) 0x20000c)
#define video_register_winx (*(volatile uint16_t*) 0x20000e)
#define video_register_winy (*(volatile uint16_t*) 0x200010)
#define video_register_winwidth (*(volatile uint16_t*) 0x200012)
#define video_register_winheight (*(volatile uint16_t*) 0x200014)
#define video_framebuffer ((volatile uint16_t*) 0x280000)
#define video_compositingbuffer ((volatile uint16_t*) 0x300000)

#endif // VIDEOREGISTERS
