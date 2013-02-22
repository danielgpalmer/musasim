/*
 * ringbuffer.h
 *
 *  Created on: 12 Dec 2012
 *      Author: daniel
 */

#ifndef RINGBUFFER_H_
#define RINGBUFFER_H_

#include <stdint.h>
#include <stdbool.h>

typedef struct {
	int16_t* buffer;
	unsigned int len;
	unsigned int head;
	unsigned int tail;
} ringbuffer;

unsigned int ringbuffer_samplesavailable(ringbuffer* buff);
unsigned int ringbuffer_samplesfree(ringbuffer* buff);
void ringbuffer_put(ringbuffer* buff, int16_t value);
int16_t ringbuffer_get(ringbuffer* buff);
int16_t ringbuffer_peek(ringbuffer* buff, unsigned int offset);
bool ringbuffer_isfull(ringbuffer* buff);
bool ringbuffer_isempty(ringbuffer* buff);
ringbuffer* ringbuffer_new(unsigned int size);
void ringbuffer_free(ringbuffer* buff);

#endif /* RINGBUFFER_H_ */
