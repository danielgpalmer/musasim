/**
 * \file sound.h
 * \author daniel
 * \brief Functions to perform common sound operations like uploading samples
 */

#include <stdint.h>

void sound_begin();
void sound_uploadsample(uint16_t* data, unsigned short bank, uint16_t bankoffset, uint16_t samplelen);
void sound_commit();
