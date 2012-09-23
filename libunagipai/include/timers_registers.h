/**
 *   \file timers_registers.h
 *   \author genheader
 *   \brief Timers register defines
 */
#ifndef LIBUNAGIPAI_TIMERSREGISTERS_H_
#define LIBUNAGIPAI_TIMERSREGISTERS_H_


#include <stdint.h>


#define timers_timer_0_flags (*(volatile uint16_t*) 0xc0000f)
#define timers_timer_0_config (*(volatile uint16_t*) 0xc00011)
#define timers_timer_0_prescaler (*(volatile uint16_t*) 0xc00013)
#define timers_timer_0_prescalercounter (*(volatile uint16_t*) 0xc00015)
#define timers_timer_0_counter (*(volatile uint16_t*) 0xc00017)
#define timers_timer_0_matcha (*(volatile uint16_t*) 0xc00019)
#define timers_timer_0_matchb (*(volatile uint16_t*) 0xc0001b)
#define timers_timer_1_flags (*(volatile uint16_t*) 0xc0001e)
#define timers_timer_1_config (*(volatile uint16_t*) 0xc00020)
#define timers_timer_1_prescaler (*(volatile uint16_t*) 0xc00022)
#define timers_timer_1_prescalercounter (*(volatile uint16_t*) 0xc00024)
#define timers_timer_1_counter (*(volatile uint16_t*) 0xc00026)
#define timers_timer_1_matcha (*(volatile uint16_t*) 0xc00028)
#define timers_timer_1_matchb (*(volatile uint16_t*) 0xc0002a)
#define timers_timer_2_flags (*(volatile uint16_t*) 0xc0002d)
#define timers_timer_2_config (*(volatile uint16_t*) 0xc0002f)
#define timers_timer_2_prescaler (*(volatile uint16_t*) 0xc00031)
#define timers_timer_2_prescalercounter (*(volatile uint16_t*) 0xc00033)
#define timers_timer_2_counter (*(volatile uint16_t*) 0xc00035)
#define timers_timer_2_matcha (*(volatile uint16_t*) 0xc00037)
#define timers_timer_2_matchb (*(volatile uint16_t*) 0xc00039)
#define timers_timer_3_flags (*(volatile uint16_t*) 0xc0003c)
#define timers_timer_3_config (*(volatile uint16_t*) 0xc0003e)
#define timers_timer_3_prescaler (*(volatile uint16_t*) 0xc00040)
#define timers_timer_3_prescalercounter (*(volatile uint16_t*) 0xc00042)
#define timers_timer_3_counter (*(volatile uint16_t*) 0xc00044)
#define timers_timer_3_matcha (*(volatile uint16_t*) 0xc00046)
#define timers_timer_3_matchb (*(volatile uint16_t*) 0xc00048)

#endif // TIMERSREGISTERS
