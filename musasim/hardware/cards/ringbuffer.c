/*
 * ringbuffer.c
 *
 *  Created on: 12 Dec 2012
 *      Author: daniel
 */

#include "ringbuffer.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>

#define BUFFERISEMPTY(a) (buff->head == buff->tail)
#define HEADISBEFORETAIL(a) (a->tail > a->head)

// how many samples are in the buffer
unsigned int ringbuffer_samplesavailable(ringbuffer* buff) {
	if (BUFFERISEMPTY(buff))
		return 0;
	else if (HEADISBEFORETAIL(buff))
		return (buff->len - buff->tail) + buff->head;
	else
		return buff->head - buff->tail;
}

// how many samples can be put in the buffer
unsigned int ringbuffer_samplesfree(ringbuffer* buff) {
	return (buff->len - ringbuffer_samplesavailable(buff)) - 1;
}

bool ringbuffer_isfull(ringbuffer* buff) {
	return ((buff->head + 1) % buff->len == buff->tail);
}

bool ringbuffer_isempty(ringbuffer* buff) {
	return (buff->head == buff->tail);
}

void ringbuffer_put(ringbuffer* buff, int16_t value) {
	printf("value %d\n", value);
	*(buff->buffer + buff->head) = value;
	buff->head = (buff->head + 1) % buff->len;
}

int16_t ringbuffer_get(ringbuffer* buff) {
	int16_t value = *(buff->buffer + buff->tail);
	buff->tail = (buff->tail + 1) % buff->len;
	return value;
}

// reset the buffer to empty
void ringbuffer_reset(ringbuffer* buff) {
	buff->head = 0;
	buff->tail = 0;
}

ringbuffer* ringbuffer_new(unsigned int size) {
	ringbuffer* buff = malloc(sizeof(ringbuffer));
	buff->len = size + 1;
	buff->buffer = malloc(buff->len * sizeof(int16_t));
	ringbuffer_reset(buff);
	return buff;
}

void ringbuffer_free(ringbuffer* buff) {
	free(buff->buffer);
	free(buff);
}
