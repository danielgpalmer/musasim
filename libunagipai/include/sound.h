/**
 * \file sound.h
 * \author daniel
 * \brief Functions to perform common sound operations like uploading samples
 */

#include <stdint.h>
#include <stdbool.h>

void sound_begin();
void sound_configuremasterchannel(bool enabled);
void sound_configurechannel(unsigned short channel, bool enabled, bool left, bool right);
void sound_uploadsample(uint16_t* data, unsigned short bank, uint16_t bankoffset, uint16_t samplelen);
void sound_setmastervolume(uint16_t mastervolume);
void sound_setchannelvolume(unsigned short channel, uint16_t channelvolume);
void sound_commit();
