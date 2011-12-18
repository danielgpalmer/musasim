/* video.h */

#include <stdint.h>


volatile uint16_t* video_start = (uint16_t*) 0x200000;
volatile uint16_t* video_end = (uint16_t*) 0x23fc00;
volatile uint16_t* video_registers = (uint16_t*) 0x240000;

