/*
 * throttler.c
 *
 *  Created on: 18 Apr 2013
 *      Author: daniel
 */

/* Todo list
 * - This won't work for multithreaded builds
 */

#include <stdbool.h>
#include <sys/time.h>

#include <math.h>
#include <stdlib.h>
#include <unistd.h>
#include <glib.h>
#include <time.h>

#include "hardware/board.h"
#include "throttler.h"
#include "config.h"
#include "sim.h"
#include "utils.h"
#include "logging.h"

#define TAG "throttler"

// RT == realtime
#define AHEADTHRESHOLD 500 // need to research how to get a good value for this,.. this needs to be big enough that we only introduce sleeps when we have actually built up lead against RT, but not too big that noticeable jumps happen. Systems might have a minimum sleep time too.
#define BEHINDTHRESHOLD 10000000 // TODO - bleh
static bool enabled = true;
static struct timespec start, end, cpustart, cpuend, cardstart, cardend;

static long owed = 0; // this will be positive when the emulation is behind RT and negative with the emulation is ahead of RT
static long count = 0;

static double speed = 0; // the speed at which the emulation is currently running, < 1 == behind RT, 1 == RT, >1 == ahead of RT
static double overhead = 0; // the amount of time per tick that wasn't rolling over the cpu or cards, stuff like updating the OSD etc

static long cputime;
static long executiontimes[NUM_SLOTS];

void throttler_starttick() {
	clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &start);
}

void throttler_startcputick() {
	clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &cpustart);
}

void throttler_endcputick() {
	clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &cpuend);
	struct timespec* diff = timespecdiff(&cpustart, &cpuend);
	cputime = (diff->tv_sec * SIM_ONENANOSECOND) + diff->tv_nsec;
}

void throttler_startcardtick(int slot) {
	clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &cardstart);
}

void throttler_endcardtick(int slot) {
	clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &cardend);
	struct timespec* diff = timespecdiff(&cardstart, &cardend);
	long time = (diff->tv_sec * SIM_ONENANOSECOND) + diff->tv_nsec;
	executiontimes[slot] = time;
}

void throttler_endtick(int cpucyclesexecuted) {
	clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &end);

	struct timespec* timediff = timespecdiff(&start, &end);
	long timetaken = (timediff->tv_sec * SIM_ONENANOSECOND) + timediff->tv_nsec;
	long target = SIM_CPUCLOCKDURATION * cpucyclesexecuted;

	double currentspeed = (float) target / (float) timetaken;

	long emulationtime = cputime;
	for (int i = 0; i < G_N_ELEMENTS(executiontimes); i++)
		emulationtime += executiontimes[i];
	double currentoverhead = (float) 1 - ((float) emulationtime / (float) timetaken);

	count++;

	speed = ((speed * (count - 1)) + currentspeed) / count;
	overhead = ((overhead * (count - 1)) + currentoverhead) / count;

	// This clamps the time taken to a max of twice the
	// expected time so things don't get out of hand
	// trying to catch up
	long clamp = target * 2;
	timetaken = MIN(clamp, timetaken);

	long diff = target - timetaken;
	if (diff > 0)
		owed -= diff; // we were ahead this tick, so take some off of our balance
	else
		owed += labs(diff); // we were behind this tick, so add to our balance;

#if !PROFILINGBUILD
	// if throttling is enabled and we're far enough ahead sleep for a little while to let realtime catch up
	if (enabled && (owed < -AHEADTHRESHOLD)) {
		static struct timespec nanosleepreq, r;
		nanosleepreq.tv_nsec = labs(owed);
		nanosleep(&nanosleepreq, &r);
		owed = -r.tv_nsec;
	}
#endif
}

void throttler_enable(bool enable) {
	enabled = enable;
	if (enable)
		log_println(LEVEL_INFO, TAG, "Throttler is now enabled");
	else
		log_println(LEVEL_INFO, TAG, "Throttler is now disabled");
}

void throttler_toggle() {
	enabled = !enabled;
}

bool throttler_behind() {
	return owed > BEHINDTHRESHOLD;
}

double throttler_speed() {
	return speed;
}

double throttler_overhead() {
	return overhead;
}
