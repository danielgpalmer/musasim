/**
 *   \file timers_registers.h
 *   \author genheader
 *   \brief Timers register defines
 */
#ifndef LIBUNAGIPAI_TIMERSREGISTERS_H_
#define LIBUNAGIPAI_TIMERSREGISTERS_H_


#include <stdint.h>


#define timers_timerinterrupts (*(volatile uint16_t*) 0xc00000)
#define timers_timer_0_flags (*(volatile uint16_t*) 0xc00020)
#define timers_timer_0_config (*(volatile uint16_t*) 0xc00022)
#define timers_timer_0_prescaler (*(volatile uint16_t*) 0xc00024)
#define timers_timer_0_prescalercounter (*(volatile uint16_t*) 0xc00026)
#define timers_timer_0_counter (*(volatile uint16_t*) 0xc00028)
#define timers_timer_0_matcha (*(volatile uint16_t*) 0xc0002a)
#define timers_timer_0_matchb (*(volatile uint16_t*) 0xc0002c)
#define timers_timer_1_flags (*(volatile uint16_t*) 0xc00020)
#define timers_timer_1_config (*(volatile uint16_t*) 0xc00022)
#define timers_timer_1_prescaler (*(volatile uint16_t*) 0xc00024)
#define timers_timer_1_prescalercounter (*(volatile uint16_t*) 0xc00026)
#define timers_timer_1_counter (*(volatile uint16_t*) 0xc00028)
#define timers_timer_1_matcha (*(volatile uint16_t*) 0xc0002a)
#define timers_timer_1_matchb (*(volatile uint16_t*) 0xc0002c)
#define timers_timer_2_flags (*(volatile uint16_t*) 0xc00020)
#define timers_timer_2_config (*(volatile uint16_t*) 0xc00022)
#define timers_timer_2_prescaler (*(volatile uint16_t*) 0xc00024)
#define timers_timer_2_prescalercounter (*(volatile uint16_t*) 0xc00026)
#define timers_timer_2_counter (*(volatile uint16_t*) 0xc00028)
#define timers_timer_2_matcha (*(volatile uint16_t*) 0xc0002a)
#define timers_timer_2_matchb (*(volatile uint16_t*) 0xc0002c)
#define timers_timer_3_flags (*(volatile uint16_t*) 0xc00020)
#define timers_timer_3_config (*(volatile uint16_t*) 0xc00022)
#define timers_timer_3_prescaler (*(volatile uint16_t*) 0xc00024)
#define timers_timer_3_prescalercounter (*(volatile uint16_t*) 0xc00026)
#define timers_timer_3_counter (*(volatile uint16_t*) 0xc00028)
#define timers_timer_3_matcha (*(volatile uint16_t*) 0xc0002a)
#define timers_timer_3_matchb (*(volatile uint16_t*) 0xc0002c)
#define timers_bigtimer_0_flags (*(volatile uint16_t*) 0xc00020)
#define timers_bigtimer_0_config (*(volatile uint16_t*) 0xc00022)
#define timers_bigtimer_0_prescaler (*(volatile uint32_t*) 0xc00024)
#define timers_bigtimer_0_prescalercounter (*(volatile uint32_t*) 0xc00028)
#define timers_bigtimer_0_counter (*(volatile uint32_t*) 0xc0002c)
#define timers_bigtimer_0_matcha (*(volatile uint32_t*) 0xc00030)
#define timers_bigtimer_0_matchb (*(volatile uint32_t*) 0xc00034)
#define timers_bigtimer_1_flags (*(volatile uint16_t*) 0xc00020)
#define timers_bigtimer_1_config (*(volatile uint16_t*) 0xc00022)
#define timers_bigtimer_1_prescaler (*(volatile uint32_t*) 0xc00024)
#define timers_bigtimer_1_prescalercounter (*(volatile uint32_t*) 0xc00028)
#define timers_bigtimer_1_counter (*(volatile uint32_t*) 0xc0002c)
#define timers_bigtimer_1_matcha (*(volatile uint32_t*) 0xc00030)
#define timers_bigtimer_1_matchb (*(volatile uint32_t*) 0xc00034)
#define timers_bigtimer_2_flags (*(volatile uint16_t*) 0xc00020)
#define timers_bigtimer_2_config (*(volatile uint16_t*) 0xc00022)
#define timers_bigtimer_2_prescaler (*(volatile uint32_t*) 0xc00024)
#define timers_bigtimer_2_prescalercounter (*(volatile uint32_t*) 0xc00028)
#define timers_bigtimer_2_counter (*(volatile uint32_t*) 0xc0002c)
#define timers_bigtimer_2_matcha (*(volatile uint32_t*) 0xc00030)
#define timers_bigtimer_2_matchb (*(volatile uint32_t*) 0xc00034)
#define timers_bigtimer_3_flags (*(volatile uint16_t*) 0xc00020)
#define timers_bigtimer_3_config (*(volatile uint16_t*) 0xc00022)
#define timers_bigtimer_3_prescaler (*(volatile uint32_t*) 0xc00024)
#define timers_bigtimer_3_prescalercounter (*(volatile uint32_t*) 0xc00028)
#define timers_bigtimer_3_counter (*(volatile uint32_t*) 0xc0002c)
#define timers_bigtimer_3_matcha (*(volatile uint32_t*) 0xc00030)
#define timers_bigtimer_3_matchb (*(volatile uint32_t*) 0xc00034)

#endif // TIMERSREGISTERS
