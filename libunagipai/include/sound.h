/**
 * \file sound.h
 * \author daniel
 * \brief Functions to perform common sound operations like uploading samples
 */

#include <stdint.h>

void sound_start();
void sound_uploadsample(uint16_t* data, unsigned int bank, uint16_t bankoffset);
void sound_commit();
