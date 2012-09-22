/**
 *   \file timers_registers.h
 *   \author genheader
 *   \brief Timers register defines
 */
#ifndef LIBUNAGIPAI_TIMERSREGISTERS_H_
#define LIBUNAGIPAI_TIMERSREGISTERS_H_


#include <stdint.h>


#define timers_timer_0_flags (*(volatile uint16_t*) 0xc00000)
#define timers_timer_0_config (*(volatile uint16_t*) 0xc00002)
#define timers_timer_0_prescaler (*(volatile uint16_t*) 0xc00004)
#define timers_timer_0_prescalercounter (*(volatile uint16_t*) 0xc00006)
#define timers_timer_0_counter (*(volatile uint16_t*) 0xc00008)
#define timers_timer_0_matcha (*(volatile uint16_t*) 0xc0000a)
#define timers_timer_0_matchb (*(volatile uint16_t*) 0xc0000c)
#define timers_timer_1_flags (*(volatile uint16_t*) 0xc00000)
#define timers_timer_1_config (*(volatile uint16_t*) 0xc00002)
#define timers_timer_1_prescaler (*(volatile uint16_t*) 0xc00004)
#define timers_timer_1_prescalercounter (*(volatile uint16_t*) 0xc00006)
#define timers_timer_1_counter (*(volatile uint16_t*) 0xc00008)
#define timers_timer_1_matcha (*(volatile uint16_t*) 0xc0000a)
#define timers_timer_1_matchb (*(volatile uint16_t*) 0xc0000c)
#define timers_timer_2_flags (*(volatile uint16_t*) 0xc00000)
#define timers_timer_2_config (*(volatile uint16_t*) 0xc00002)
#define timers_timer_2_prescaler (*(volatile uint16_t*) 0xc00004)
#define timers_timer_2_prescalercounter (*(volatile uint16_t*) 0xc00006)
#define timers_timer_2_counter (*(volatile uint16_t*) 0xc00008)
#define timers_timer_2_matcha (*(volatile uint16_t*) 0xc0000a)
#define timers_timer_2_matchb (*(volatile uint16_t*) 0xc0000c)
#define timers_timer_3_flags (*(volatile uint16_t*) 0xc00000)
#define timers_timer_3_config (*(volatile uint16_t*) 0xc00002)
#define timers_timer_3_prescaler (*(volatile uint16_t*) 0xc00004)
#define timers_timer_3_prescalercounter (*(volatile uint16_t*) 0xc00006)
#define timers_timer_3_counter (*(volatile uint16_t*) 0xc00008)
#define timers_timer_3_matcha (*(volatile uint16_t*) 0xc0000a)
#define timers_timer_3_matchb (*(volatile uint16_t*) 0xc0000c)

#endif // TIMERSREGISTERS
