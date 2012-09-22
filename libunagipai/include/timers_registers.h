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

#endif // TIMERSREGISTERS
