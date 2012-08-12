/*
 * dmacard.c
 *
 *  Created on: Dec 6, 2011
 *      Author: daniel
 */

#include <string.h>
#include <dma_registermasks.h>

#include "dmacard.h"
#include <stdint.h>
#include <stdbool.h>
#include "../../logging.h"
#include "../../sim.h"
#include "../board.h"

#define NUMWINDOWS 16
#define ADDRESSMASK 0x1f
static const char TAG[] = "dmacard";
static char* register_names[] = { "config", "data high", "data low", "count h", "count low", "source high",
		"source low", "destination h", "destination low", "jump after", "jump length", "window" };

// Externally visible registers

typedef struct {
	uint16_t config;
	uint16_t datal;
	uint16_t datah;
	uint16_t countl;
	uint16_t counth;
	uint16_t sourcel;
	uint16_t sourceh;
	uint16_t destinationl;
	uint16_t destinationh;
	uint16_t jumpafter;
	uint16_t jumplength;
} window;

static void dmacard_dumpconfig(window* window);

static window* curwindow;
static uint16_t windowpointer = 0;
static uint16_t curwindowindex = 0;
static window regwindows[NUMWINDOWS];

// Latched values
static uint32_t counter = 0;
static uint32_t data = 0;
static uint32_t source = 0;
static uint32_t destination = 0;

// State
static bool done = false;
static bool started = false;
static bool wordtransfer = false;
static bool havebuslock = false;
static uint32_t cyclesleft = 0;

static void dmacard_init() {
	memset(regwindows, 0, sizeof(regwindows));
	curwindow = &(regwindows[curwindowindex]);
}

static void dmacard_irqack() {

}

static void dmacard_buslock() {
	log_println(LEVEL_DEBUG, TAG, "dmacard_buslock");
	board_lock_bus(&dmacard);
}

static void dmacard_busgrant() {
	log_println(LEVEL_DEBUG, TAG, "dmacard_busgrant");
	havebuslock = true;
}

static void dmacard_busrelease() {
	log_println(LEVEL_DEBUG, TAG, "dmacard_busrelease");
	board_unlock_bus(&dmacard);
	havebuslock = false;
}

static uint16_t dmacard_mutate(window* window, uint16_t value1, uint16_t value2) {

	switch (window->config & DMA_REGISTER_CONFIG_MUTATOR) {
		case DMA_MUT_NOTHING:
			return value1;
		case DMA_MUT_AND:
			return value1 & value2;
		case DMA_MUT_OR:
			return value1 | value2;
		case DMA_MUT_XOR:
			return value1 ^ value2;
	}

	// should never get here
	return value1;

}

static void dmacard_perform_act(window* window, int index) {
	static uint32_t* actregs[] = { &data, &source, &destination };
	static int actshifts[] = { DMA_REGISTER_CONFIG_DATAACT_SHIFT, DMA_REGISTER_CONFIG_SRCACT_SHIFT,
			DMA_REGISTER_CONFIG_DSTACT_SHIFT };

	uint32_t* reg = actregs[index];

	int act = (window->config & (DMA_REGISTER_CONFIG_ACT << actshifts[index])) >> actshifts[index];

	switch (act) {
		case DMA_ACT_NOTHING:
			break;
		case DMA_ACT_INC:
			wordtransfer ? (*reg) += 2 : (*reg)++;
			break;
		case DMA_ACT_DEC:
			wordtransfer ? (*reg) -= 2 : (*reg)--;
			break;
		case DMA_ACT_ROTLEFT:
			(*reg) = ((*reg) << 1) & 0xffff;
			break;
		case DMA_ACT_ROTRIGHT:
			(*reg) = ((*reg) >> 1) & 0xffff;
			break;
		case DMA_ACT_INVERSE:
			(*reg) = ~(*reg);
			break;
	}
}

#define FULLCOUNTER(w) ((w->counth << 16) | w->countl)

static window* workingwindow;
static void dmacard_popwindow() {

	workingwindow = &(regwindows[windowpointer]);
	counter = FULLCOUNTER(workingwindow);
	source = ((workingwindow->sourceh << 16) | workingwindow->sourcel);
	destination = ((workingwindow->destinationh << 16) | workingwindow->destinationl);
	data = ((workingwindow->datah << 16) | workingwindow->datal);
	wordtransfer = workingwindow->config & DMA_REGISTER_CONFIG_SIZE;
	windowpointer++;

	dmacard_dumpconfig(workingwindow);

}

static bool transferinprogress = false;

static void dmacard_tick(int cyclesexecuted) {

	static int state = 0; // for transfer modes to track their current phase of the unit
	static uint16_t holding = 0; // for stashing data between phases

	// A DMA session hasn't been started
	if (!started) {
		return;
	}

	// Triggered and bus requested but not locked yet
	else if (!havebuslock) {
		return;
	}

	for (int i = 0; i < cyclesexecuted; i++) {

		cyclesleft--;

		bool unitcomplete = false;

		if (!transferinprogress) {
			// no transfer in progress, but still register windows to handle
			if (windowpointer != curwindowindex) {
				dmacard_popwindow();
				transferinprogress = true;
			}
			// all the register windows are gone and there is no transfer.. time to go home
			else {
				started = false;
				done = true;
				dmacard_busrelease();
				break;
			}
		}

		else {
			if (counter == 0) {
				log_println(LEVEL_DEBUG, TAG, "transfer done");
				transferinprogress = false;
			}

			else {
				//log_println(LEVEL_DEBUG, TAG, "source[0%08x] dest[0x%08x]\n", source, destination);

				switch (workingwindow->config & DMA_REGISTER_CONFIG_MODE) {
					case DMA_REGISTER_CONFIG_MODE_BLOCK: // Takes two clocks, one for read, one for write
						// read phase
						if (state == 0) {
							if (wordtransfer) {
								holding = board_read_word(source);
							}
							else {
								holding = board_read_byte(source);
							}
							state = 1;
						}
						//write phase
						else {
							if (wordtransfer) {
								board_write_word(destination, dmacard_mutate(workingwindow, holding, data));
							}
							else {
								board_write_byte(destination,
										(uint8_t) (dmacard_mutate(workingwindow, holding, data) & 0xff));
							}
							state = 0;
							unitcomplete = true;
						}
						break;

					case DMA_REGISTER_CONFIG_MODE_FILL: // Takes one clock
						if (wordtransfer) {
							board_write_word(destination, (uint16_t) (data & 0xffff));
						}
						else {
							board_write_byte(destination, (uint8_t) (data & 0xff));
						}
						unitcomplete = true;
						break;

					case DMA_REGISTER_CONFIG_MODE_MIX: // Takes three clocks?

						switch (state) {
							case 0:
								if (wordtransfer) {
									holding = board_read_word(source);
								}
								else {
									holding = board_read_byte(source);
								}
								state = 1;
								break;
							case 1:
								if (wordtransfer) {
									holding = dmacard_mutate(workingwindow, holding, board_read_word(data));
								}
								else {
									holding = dmacard_mutate(workingwindow, holding, board_read_byte(data));
								}
								state = 2;
								break;
							case 2:
								if (wordtransfer) {
									board_write_word(destination, holding);
								}
								else {
									board_write_byte(destination, holding);
								}
								state = 0;
								unitcomplete = true;
								break;
						}

						break;
					case DMA_REGISTER_CONFIG_MODE_MIXCOMPACT: {
						static int shifts = 0;
						static uint16_t shifthold = 0;
						switch (state) {
							case 0: // load the src

								if (wordtransfer) {
									holding = board_read_word(source);
								}
								dmacard_perform_act(workingwindow, 1); // update source pointer
								if (shifts == 0) {
									state = 1;
								}
								else {
									state = 2;
								}
								break;
							case 1: // load the mask
								if (wordtransfer) {
									shifthold = board_read_word(data);
								}
								state = 2;
								dmacard_perform_act(workingwindow, 0); // update data pointer
								break;
							case 2:
								holding = dmacard_mutate(workingwindow, holding, shifthold & 0x1 ? 0xFFFF : 0x0000); // if the lsb bit is set do the action with 0xffff
								shifthold = (shifthold >> 1); // shift everything right
								shifts++;

								// next shift round
								if ((wordtransfer && shifts == 16) || shifts == 8) {
									shifts = 0;
									counter--;
								}

								if (wordtransfer) {
									board_write_word(destination, holding);
								}
								else {
									board_write_byte(destination, holding);
								}
								dmacard_perform_act(workingwindow, 2); // increment dest pointer
								state = 0;
								break;
						}
					}
						break;

				}

				// Update all the pointers .. used for everything but MIXCOMPACT
				if (unitcomplete) {
					// Perform actions
					for (int i = 0; i < 3; i++) {
						dmacard_perform_act(workingwindow, i);
					}

					//log_println(LEVEL_DEBUG, TAG, "data 0x%08x, src 0x%08x, dst 0x%08x", datalatched, source, destination);

					counter--;
					if (workingwindow->jumpafter != 0) {
						if (counter % workingwindow->jumpafter == 0) {
							destination += (workingwindow->jumplength * 2);
						}
					}
				}
			}

		}

	}
}

static uint16_t* dmacard_decodereg(uint32_t address) {
	int reg = (address & ADDRESSMASK);

	switch (reg) {
		case DMACARD_REGISTER_CONFIG:
			return &(curwindow->config);
		case DMACARD_REGISTER_DATAH:
			return &(curwindow->datah);
		case DMACARD_REGISTER_DATAL:
			return &(curwindow->datal);
		case DMACARD_REGISTER_COUNTH:
			return &(curwindow->counth);
		case DMACARD_REGISTER_COUNTL:
			return &(curwindow->countl);
		case DMACARD_REGISTER_SOURCEH:
			return &(curwindow->sourceh);
		case DMACARD_REGISTER_SOURCEL:
			return &(curwindow->sourcel);
		case DMACARD_REGISTER_DESTINATIONH:
			return &(curwindow->destinationh);
		case DMACARD_REGISTER_DESTINATIONL:
			return &(curwindow->destinationl);
		case DMACARD_REGISTER_JUMPAFTER:
			return &(curwindow->jumpafter);
		case DMACARD_REGISTER_JUMPLENGTH:
			return &(curwindow->jumplength);
		case DMACARD_REGISTER_WINDOW:
			return &curwindowindex;
		default:
			return NULL;
	}

}

static int dmacard_howmanycycles(window* window) {
	uint16_t config = window->config;
	switch (config & DMA_REGISTER_CONFIG_MODE) {
		case DMA_REGISTER_CONFIG_MODE_FILL:
			return FULLCOUNTER(window);
		case DMA_REGISTER_CONFIG_MODE_BLOCK:
			return FULLCOUNTER(window) * 2;
	}
	return 1;
}

static void dmacard_dumpconfig(window* window) {
	uint16_t config = window->config;

	int cycles = dmacard_howmanycycles(window);

	switch (config & DMA_REGISTER_CONFIG_MODE) {
		case DMA_REGISTER_CONFIG_MODE_FILL:
			log_println(LEVEL_DEBUG, TAG, "copying data [0x%04x] 0x%08x times as %s to 0x%08x, will take %d cycles",
					data, counter, config & DMA_REGISTER_CONFIG_SIZE ? "words" : "bytes", destination, cycles);
			break;
		case DMA_REGISTER_CONFIG_MODE_BLOCK:
			log_println(LEVEL_DEBUG, TAG, "transferring 0x%08x %s from 0x%08x to 0x%08x, will take %d cycles", counter,
					config & DMA_REGISTER_CONFIG_SIZE ? "words" : "bytes", source, destination, cycles);
			break;
	}

	if (config & DMA_REGISTER_CONFIG_MUTATOR) {
		log_println(LEVEL_DEBUG, TAG, "data mutation is going to happen");
	}

	if (config & DMA_REGISTER_CONFIG_DSTACT_INC) {
		log_println(LEVEL_DEBUG, TAG, "dst will increment by two");
	}

}

static uint16_t dmacard_read_word(uint32_t address) {
	uint16_t value = *(dmacard_decodereg(address));

	int reg = (address & ADDRESSMASK);
	if (reg == DMACARD_REGISTER_CONFIG) {
		if (done) {
			value |= DMA_REGISTER_CONFIG_DONE;
		}
	}

	return value;
}

static void dmacard_start() {
	windowpointer = 0;
	started = true;
	done = false;
	dmacard_buslock();

	cyclesleft = 0;
	for (int i = 0; i <= curwindowindex; i++) {
		cyclesleft += dmacard_howmanycycles(&regwindows[i]);
	}
	log_println(LEVEL_INFO, TAG, "total cycles %d", cyclesleft);
}

static void dmacard_write_word(uint32_t address, uint16_t value) {

// DMA transfer was started
	if (started) {
		log_println(LEVEL_INFO, TAG, "Someone tried to write registers while a transfer is in the progress!");
		return;
	}

	int reg = (address & ADDRESSMASK);
	log_println(LEVEL_DEBUG, TAG, "write 0x%04x [%s], value 0x%04x", address,
			reg == 0 ? register_names[0] : register_names[reg / 2], value);

	if (reg == DMACARD_REGISTER_CONFIG) {
		// start has been triggered, get this show rolling
		if (value & DMA_REGISTER_CONFIG_START) {
			dmacard_start();
		}
		// strip off the start and done bits if set
		value &= 0x3fff;
	}

	*(dmacard_decodereg(address)) = value;

	// handle changing the register window
	if (reg == DMACARD_REGISTER_WINDOW) {
		log_println(LEVEL_DEBUG, TAG, "Window changed to %d", curwindowindex);
		curwindow = &regwindows[curwindowindex];
	}
}

static bool dmacard_validaddress(uint32_t address) {

	int reg = (address & ADDRESSMASK);

	switch (reg) {
		case DMACARD_REGISTER_CONFIG:
		case DMACARD_REGISTER_DATAH:
		case DMACARD_REGISTER_DATAL:
		case DMACARD_REGISTER_COUNTH:
		case DMACARD_REGISTER_COUNTL:
		case DMACARD_REGISTER_SOURCEH:
		case DMACARD_REGISTER_SOURCEL:
		case DMACARD_REGISTER_DESTINATIONH:
		case DMACARD_REGISTER_DESTINATIONL:
		case DMACARD_REGISTER_JUMPAFTER:
		case DMACARD_REGISTER_JUMPLENGTH:
		case DMACARD_REGISTER_WINDOW:
			return true;
		default:
			return false;
	}
}

static bool dmacard_active() {
	return transferinprogress;
}

static int dmacard_cyclesleft() {
	if (!started)
		return -1;

	log_println(LEVEL_INFO, TAG, "cycles left %d", cyclesleft);
	return cyclesleft;
}

const card dmacard = { "DMA Controller", //
		dmacard_init, //
		NULL, //
		NULL, //
		dmacard_tick, //
		dmacard_irqack, //
		dmacard_busgrant, //
		dmacard_validaddress, //
		NULL, //
		dmacard_read_word, //
		NULL, //
		NULL, //
		dmacard_write_word, //
		NULL, //
		dmacard_active, //
		NULL, //
		dmacard_cyclesleft };

