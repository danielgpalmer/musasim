/*
 * rtc.c
 *
 *  Created on: 23 Sep 2012
 *      Author: daniel
 */

#define TAG "rtcmodule"

#include <stddef.h>
#include "rtc.h"

static void* rtc_init(module_callback* callback, int index) {

	return NULL ;

}

const module rtcmodule = { //
		TAG, //
				NULL,//
				rtc_init, //
				NULL, //
				NULL, //
				NULL, //
				NULL, //
				NULL, //
				NULL, //
				NULL, //
				NULL //
		};
