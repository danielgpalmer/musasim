/*
 * dmacard.c
 *
 *  Created on: Dec 6, 2011
 *      Author: daniel
 */

#ifndef DMACARD_C_
#define DMACARD_C_

#include "dmacard.h"

void dmacard_init() {

}

void dmacard_dispose() {

}

void dmacard_irqack() {

}

void dmacard_busgrant() {

}

void dmacard_tick() {

}

card dmacard = { "DMA Controller", dmacard_init, dmacard_dispose, dmacard_tick, dmacard_irqack, dmacard_busgrant, NULL,
		NULL, NULL, NULL, NULL, NULL };

#endif /* DMACARD_C_ */
