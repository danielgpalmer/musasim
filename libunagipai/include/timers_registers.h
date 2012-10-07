/**
 *   \file timers_registers.h
 *   \author genheader
 *   \brief Timers register defines
 */
#ifndef LIBUNAGIPAI_TIMERSREGISTERS_H_
#define LIBUNAGIPAI_TIMERSREGISTERS_H_


#include <stdint.h>


#define timers_timerinterrupts (*(volatile uint16_t*) 0xc00000)
#define timers_timer_0_flags (*(volatile uint16_t*) 0xc00002)
#define timers_timer_0_config (*(volatile uint16_t*) 0xc00004)
#define timers_timer_0_prescaler (*(volatile uint16_t*) 0xc00006)
#define timers_timer_0_prescalercounter (*(volatile uint16_t*) 0xc00008)
#define timers_timer_0_counter (*(volatile uint16_t*) 0xc0000a)
#define timers_timer_0_matcha (*(volatile uint16_t*) 0xc0000c)
#define timers_timer_0_matchb (*(volatile uint16_t*) 0xc0000e)
#define timers_timer_1_flags (*(volatile uint16_t*) 0xc00012)
#define timers_timer_1_config (*(volatile uint16_t*) 0xc00014)
#define timers_timer_1_prescaler (*(volatile uint16_t*) 0xc00016)
#define timers_timer_1_prescalercounter (*(volatile uint16_t*) 0xc00018)
#define timers_timer_1_counter (*(volatile uint16_t*) 0xc0001a)
#define timers_timer_1_matcha (*(volatile uint16_t*) 0xc0001c)
#define timers_timer_1_matchb (*(volatile uint16_t*) 0xc0001e)
#define timers_timer_2_flags (*(volatile uint16_t*) 0xc00022)
#define timers_timer_2_config (*(volatile uint16_t*) 0xc00024)
#define timers_timer_2_prescaler (*(volatile uint16_t*) 0xc00026)
#define timers_timer_2_prescalercounter (*(volatile uint16_t*) 0xc00028)
#define timers_timer_2_counter (*(volatile uint16_t*) 0xc0002a)
#define timers_timer_2_matcha (*(volatile uint16_t*) 0xc0002c)
#define timers_timer_2_matchb (*(volatile uint16_t*) 0xc0002e)
#define timers_timer_3_flags (*(volatile uint16_t*) 0xc00032)
#define timers_timer_3_config (*(volatile uint16_t*) 0xc00034)
#define timers_timer_3_prescaler (*(volatile uint16_t*) 0xc00036)
#define timers_timer_3_prescalercounter (*(volatile uint16_t*) 0xc00038)
#define timers_timer_3_counter (*(volatile uint16_t*) 0xc0003a)
#define timers_timer_3_matcha (*(volatile uint16_t*) 0xc0003c)
#define timers_timer_3_matchb (*(volatile uint16_t*) 0xc0003e)
#define timers_bigtimer_0_flags (*(volatile uint16_t*) 0xc00042)
#define timers_bigtimer_0_config (*(volatile uint16_t*) 0xc00044)
#define timers_bigtimer_0_prescaler (*(volatile uint32_t*) 0xc00046)
#define timers_bigtimer_0_prescalercounter (*(volatile uint32_t*) 0xc0004a)
#define timers_bigtimer_0_counter (*(volatile uint32_t*) 0xc0004e)
#define timers_bigtimer_0_matcha (*(volatile uint32_t*) 0xc00052)
#define timers_bigtimer_0_matchb (*(volatile uint32_t*) 0xc00056)
#define timers_bigtimer_1_flags (*(volatile uint16_t*) 0xc00062)
#define timers_bigtimer_1_config (*(volatile uint16_t*) 0xc00064)
#define timers_bigtimer_1_prescaler (*(volatile uint32_t*) 0xc00066)
#define timers_bigtimer_1_prescalercounter (*(volatile uint32_t*) 0xc0006a)
#define timers_bigtimer_1_counter (*(volatile uint32_t*) 0xc0006e)
#define timers_bigtimer_1_matcha (*(volatile uint32_t*) 0xc00072)
#define timers_bigtimer_1_matchb (*(volatile uint32_t*) 0xc00076)
#define timers_bigtimer_2_flags (*(volatile uint16_t*) 0xc00082)
#define timers_bigtimer_2_config (*(volatile uint16_t*) 0xc00084)
#define timers_bigtimer_2_prescaler (*(volatile uint32_t*) 0xc00086)
#define timers_bigtimer_2_prescalercounter (*(volatile uint32_t*) 0xc0008a)
#define timers_bigtimer_2_counter (*(volatile uint32_t*) 0xc0008e)
#define timers_bigtimer_2_matcha (*(volatile uint32_t*) 0xc00092)
#define timers_bigtimer_2_matchb (*(volatile uint32_t*) 0xc00096)
#define timers_bigtimer_3_flags (*(volatile uint16_t*) 0xc000a2)
#define timers_bigtimer_3_config (*(volatile uint16_t*) 0xc000a4)
#define timers_bigtimer_3_prescaler (*(volatile uint32_t*) 0xc000a6)
#define timers_bigtimer_3_prescalercounter (*(volatile uint32_t*) 0xc000aa)
#define timers_bigtimer_3_counter (*(volatile uint32_t*) 0xc000ae)
#define timers_bigtimer_3_matcha (*(volatile uint32_t*) 0xc000b2)
#define timers_bigtimer_3_matchb (*(volatile uint32_t*) 0xc000b6)

#endif // TIMERSREGISTERS
