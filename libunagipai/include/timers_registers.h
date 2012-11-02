/**
 *   \file timers_registers.h
 *   \author genheader
 *   \brief Timers register defines
 */
#ifndef LIBUNAGIPAI_TIMERSREGISTERS_H_
#define LIBUNAGIPAI_TIMERSREGISTERS_H_


#include <stdint.h>


/**
0x00000000 -> 0x00000001 - block, 2 bytes
0x00000020 -> 0x0000002d - peripheral, 14 bytes
\   0x00000000 -> 0x0000000c - register group,bytes 14
 \_ 0x00000000 - flags
 \_ 0x00000002 - config
 \_ 0x00000004 - prescaler
 \_ 0x00000006 - prescalercounter
 \_ 0x00000008 - counter
 \_ 0x0000000a - matcha
 \_ 0x0000000c - matchb
0x00000040 -> 0x0000004d - peripheral, 14 bytes
\   0x00000000 -> 0x0000000c - register group,bytes 14
 \_ 0x00000000 - flags
 \_ 0x00000002 - config
 \_ 0x00000004 - prescaler
 \_ 0x00000006 - prescalercounter
 \_ 0x00000008 - counter
 \_ 0x0000000a - matcha
 \_ 0x0000000c - matchb
0x00000060 -> 0x0000006d - peripheral, 14 bytes
\   0x00000000 -> 0x0000000c - register group,bytes 14
 \_ 0x00000000 - flags
 \_ 0x00000002 - config
 \_ 0x00000004 - prescaler
 \_ 0x00000006 - prescalercounter
 \_ 0x00000008 - counter
 \_ 0x0000000a - matcha
 \_ 0x0000000c - matchb
0x00000080 -> 0x0000008d - peripheral, 14 bytes
\   0x00000000 -> 0x0000000c - register group,bytes 14
 \_ 0x00000000 - flags
 \_ 0x00000002 - config
 \_ 0x00000004 - prescaler
 \_ 0x00000006 - prescalercounter
 \_ 0x00000008 - counter
 \_ 0x0000000a - matcha
 \_ 0x0000000c - matchb
0x000000a0 -> 0x000000b7 - peripheral, 24 bytes
\   0x00000000 -> 0x00000002 - register group,bytes 4
 \_ 0x00000000 - flags
 \_ 0x00000002 - config
\   0x00000004 -> 0x00000014 - register group,bytes 20
 \_ 0x00000004 - prescaler
 \_ 0x00000008 - prescalercounter
 \_ 0x0000000c - counter
 \_ 0x00000010 - matcha
 \_ 0x00000014 - matchb
0x000000c0 -> 0x000000d7 - peripheral, 24 bytes
\   0x00000000 -> 0x00000002 - register group,bytes 4
 \_ 0x00000000 - flags
 \_ 0x00000002 - config
\   0x00000004 -> 0x00000014 - register group,bytes 20
 \_ 0x00000004 - prescaler
 \_ 0x00000008 - prescalercounter
 \_ 0x0000000c - counter
 \_ 0x00000010 - matcha
 \_ 0x00000014 - matchb
0x000000e0 -> 0x000000f7 - peripheral, 24 bytes
\   0x00000000 -> 0x00000002 - register group,bytes 4
 \_ 0x00000000 - flags
 \_ 0x00000002 - config
\   0x00000004 -> 0x00000014 - register group,bytes 20
 \_ 0x00000004 - prescaler
 \_ 0x00000008 - prescalercounter
 \_ 0x0000000c - counter
 \_ 0x00000010 - matcha
 \_ 0x00000014 - matchb
0x00000100 -> 0x00000117 - peripheral, 24 bytes
\   0x00000000 -> 0x00000002 - register group,bytes 4
 \_ 0x00000000 - flags
 \_ 0x00000002 - config
\   0x00000004 -> 0x00000014 - register group,bytes 20
 \_ 0x00000004 - prescaler
 \_ 0x00000008 - prescalercounter
 \_ 0x0000000c - counter
 \_ 0x00000010 - matcha
 \_ 0x00000014 - matchb
**/
#define timers_timerinterrupts (*(volatile uint16_t*) 0xc00000)
/* (null) */

/* timer_0 */
#define timers_timer_0_flags (*(volatile uint16_t*) 0xc00020)
#define timers_timer_0_config (*(volatile uint16_t*) 0xc00022)
#define timers_timer_0_prescaler (*(volatile uint16_t*) 0xc00024)
#define timers_timer_0_prescalercounter (*(volatile uint16_t*) 0xc00026)
#define timers_timer_0_counter (*(volatile uint16_t*) 0xc00028)
#define timers_timer_0_matcha (*(volatile uint16_t*) 0xc0002a)
#define timers_timer_0_matchb (*(volatile uint16_t*) 0xc0002c)

/* timer_1 */
#define timers_timer_1_flags (*(volatile uint16_t*) 0xc00040)
#define timers_timer_1_config (*(volatile uint16_t*) 0xc00042)
#define timers_timer_1_prescaler (*(volatile uint16_t*) 0xc00044)
#define timers_timer_1_prescalercounter (*(volatile uint16_t*) 0xc00046)
#define timers_timer_1_counter (*(volatile uint16_t*) 0xc00048)
#define timers_timer_1_matcha (*(volatile uint16_t*) 0xc0004a)
#define timers_timer_1_matchb (*(volatile uint16_t*) 0xc0004c)

/* timer_2 */
#define timers_timer_2_flags (*(volatile uint16_t*) 0xc00060)
#define timers_timer_2_config (*(volatile uint16_t*) 0xc00062)
#define timers_timer_2_prescaler (*(volatile uint16_t*) 0xc00064)
#define timers_timer_2_prescalercounter (*(volatile uint16_t*) 0xc00066)
#define timers_timer_2_counter (*(volatile uint16_t*) 0xc00068)
#define timers_timer_2_matcha (*(volatile uint16_t*) 0xc0006a)
#define timers_timer_2_matchb (*(volatile uint16_t*) 0xc0006c)

/* timer_3 */
#define timers_timer_3_flags (*(volatile uint16_t*) 0xc00080)
#define timers_timer_3_config (*(volatile uint16_t*) 0xc00082)
#define timers_timer_3_prescaler (*(volatile uint16_t*) 0xc00084)
#define timers_timer_3_prescalercounter (*(volatile uint16_t*) 0xc00086)
#define timers_timer_3_counter (*(volatile uint16_t*) 0xc00088)
#define timers_timer_3_matcha (*(volatile uint16_t*) 0xc0008a)
#define timers_timer_3_matchb (*(volatile uint16_t*) 0xc0008c)

/* bigtimer_0 */
#define timers_bigtimer_0_flags (*(volatile uint16_t*) 0xc000a0)
#define timers_bigtimer_0_config (*(volatile uint16_t*) 0xc000a2)
#define timers_bigtimer_0_prescaler (*(volatile uint32_t*) 0xc000a4)
#define timers_bigtimer_0_prescalercounter (*(volatile uint32_t*) 0xc000a8)
#define timers_bigtimer_0_counter (*(volatile uint32_t*) 0xc000ac)
#define timers_bigtimer_0_matcha (*(volatile uint32_t*) 0xc000b0)
#define timers_bigtimer_0_matchb (*(volatile uint32_t*) 0xc000b4)

/* bigtimer_1 */
#define timers_bigtimer_1_flags (*(volatile uint16_t*) 0xc000c0)
#define timers_bigtimer_1_config (*(volatile uint16_t*) 0xc000c2)
#define timers_bigtimer_1_prescaler (*(volatile uint32_t*) 0xc000c4)
#define timers_bigtimer_1_prescalercounter (*(volatile uint32_t*) 0xc000c8)
#define timers_bigtimer_1_counter (*(volatile uint32_t*) 0xc000cc)
#define timers_bigtimer_1_matcha (*(volatile uint32_t*) 0xc000d0)
#define timers_bigtimer_1_matchb (*(volatile uint32_t*) 0xc000d4)

/* bigtimer_2 */
#define timers_bigtimer_2_flags (*(volatile uint16_t*) 0xc000e0)
#define timers_bigtimer_2_config (*(volatile uint16_t*) 0xc000e2)
#define timers_bigtimer_2_prescaler (*(volatile uint32_t*) 0xc000e4)
#define timers_bigtimer_2_prescalercounter (*(volatile uint32_t*) 0xc000e8)
#define timers_bigtimer_2_counter (*(volatile uint32_t*) 0xc000ec)
#define timers_bigtimer_2_matcha (*(volatile uint32_t*) 0xc000f0)
#define timers_bigtimer_2_matchb (*(volatile uint32_t*) 0xc000f4)

/* bigtimer_3 */
#define timers_bigtimer_3_flags (*(volatile uint16_t*) 0xc00100)
#define timers_bigtimer_3_config (*(volatile uint16_t*) 0xc00102)
#define timers_bigtimer_3_prescaler (*(volatile uint32_t*) 0xc00104)
#define timers_bigtimer_3_prescalercounter (*(volatile uint32_t*) 0xc00108)
#define timers_bigtimer_3_counter (*(volatile uint32_t*) 0xc0010c)
#define timers_bigtimer_3_matcha (*(volatile uint32_t*) 0xc00110)
#define timers_bigtimer_3_matchb (*(volatile uint32_t*) 0xc00114)



#endif // TIMERSREGISTERS
