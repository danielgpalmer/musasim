/**
 *   \file sound_registers.h
 *   \author genheader
 *   \brief Sound register defines
 */
#ifndef LIBUNAGIPAI_SOUNDREGISTERS_H_
#define LIBUNAGIPAI_SOUNDREGISTERS_H_


#include <stdint.h>


#define sound_bank_0 ((volatile uint16_t*) 0x600000)
#define sound_bank_1 ((volatile uint16_t*) 0x60ffff)
#define sound_bank_2 ((volatile uint16_t*) 0x61fffe)
#define sound_bank_3 ((volatile uint16_t*) 0x62fffd)
#define sound_channel_master_config (*(volatile uint16_t*) 0x640000)
#define sound_channel_master_volume (*(volatile uint16_t*) 0x640002)
#define sound_channel_0_config (*(volatile uint16_t*) 0x640010)
#define sound_channel_0_volume (*(volatile uint16_t*) 0x640012)
#define sound_channel_0_samplepointer (*(volatile uint16_t*) 0x640014)
#define sound_channel_0_samplelength (*(volatile uint16_t*) 0x640016)
#define sound_channel_0_samplepos (*(volatile uint16_t*) 0x640018)
#define sound_channel_1_config (*(volatile uint16_t*) 0x640020)
#define sound_channel_1_volume (*(volatile uint16_t*) 0x640022)
#define sound_channel_1_samplepointer (*(volatile uint16_t*) 0x640024)
#define sound_channel_1_samplelength (*(volatile uint16_t*) 0x640026)
#define sound_channel_1_samplepos (*(volatile uint16_t*) 0x640028)
#define sound_channel_2_config (*(volatile uint16_t*) 0x640030)
#define sound_channel_2_volume (*(volatile uint16_t*) 0x640032)
#define sound_channel_2_samplepointer (*(volatile uint16_t*) 0x640034)
#define sound_channel_2_samplelength (*(volatile uint16_t*) 0x640036)
#define sound_channel_2_samplepos (*(volatile uint16_t*) 0x640038)
#define sound_channel_3_config (*(volatile uint16_t*) 0x640040)
#define sound_channel_3_volume (*(volatile uint16_t*) 0x640042)
#define sound_channel_3_samplepointer (*(volatile uint16_t*) 0x640044)
#define sound_channel_3_samplelength (*(volatile uint16_t*) 0x640046)
#define sound_channel_3_samplepos (*(volatile uint16_t*) 0x640048)
#define sound_channel_4_config (*(volatile uint16_t*) 0x640050)
#define sound_channel_4_volume (*(volatile uint16_t*) 0x640052)
#define sound_channel_4_samplepointer (*(volatile uint16_t*) 0x640054)
#define sound_channel_4_samplelength (*(volatile uint16_t*) 0x640056)
#define sound_channel_4_samplepos (*(volatile uint16_t*) 0x640058)
#define sound_channel_5_config (*(volatile uint16_t*) 0x640060)
#define sound_channel_5_volume (*(volatile uint16_t*) 0x640062)
#define sound_channel_5_samplepointer (*(volatile uint16_t*) 0x640064)
#define sound_channel_5_samplelength (*(volatile uint16_t*) 0x640066)
#define sound_channel_5_samplepos (*(volatile uint16_t*) 0x640068)
#define sound_channel_6_config (*(volatile uint16_t*) 0x640070)
#define sound_channel_6_volume (*(volatile uint16_t*) 0x640072)
#define sound_channel_6_samplepointer (*(volatile uint16_t*) 0x640074)
#define sound_channel_6_samplelength (*(volatile uint16_t*) 0x640076)
#define sound_channel_6_samplepos (*(volatile uint16_t*) 0x640078)
#define sound_channel_7_config (*(volatile uint16_t*) 0x640080)
#define sound_channel_7_volume (*(volatile uint16_t*) 0x640082)
#define sound_channel_7_samplepointer (*(volatile uint16_t*) 0x640084)
#define sound_channel_7_samplelength (*(volatile uint16_t*) 0x640086)
#define sound_channel_7_samplepos (*(volatile uint16_t*) 0x640088)

#endif // SOUNDREGISTERS
