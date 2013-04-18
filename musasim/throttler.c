/*
 * throttler.c
 *
 *  Created on: 18 Apr 2013
 *      Author: daniel
 */

#include <stdbool.h>
#include <sys/time.h>
#include <time.h>
#include <math.h>
#include <stdlib.h>
#include <unistd.h>

#include "throttler.h"
#include "sim.h"
#include "utils.h"

static bool enabled = true;
static struct timespec start, end;
static long int owed = 0;
static long count = 0;
static double speed = 0;

void throttler_starttick() {
	clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &start);
}

void throttler_endtick(int cpucyclesexecuted) {
	clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &end);
	struct timespec* timediff = timespecdiff(&start, &end);
	long timetaken = (timediff->tv_sec * SIM_ONENANOSECOND) + timediff->tv_nsec;
	long target = SIM_CPUCLOCKDURATION * cpucyclesexecuted;

	double currentspeed = (float) target / (float) timetaken;
	count++;

	speed = ((speed * (count - 1)) + currentspeed) / count;

	if (timetaken > 100000) {
		//	//log_println(LEVEL_INFO, TAG, "CLAMP!");
		timetaken = 100000;
	}

	long int diff = target - timetaken;
	if (diff > 0) {
		owed -= diff;
		if (owed < -100) {
			usleep(abs(owed) / (1000 + 100));
			owed = 0;
		}
	}
	else {
		owed += labs(diff);
	}
}

void throttler_enable(bool enable) {
	enabled = enable;
}

void throttler_toggle() {
	enabled = !enabled;
}

bool throttler_behind() {
	return owed > 0;
}

double throttler_speed() {
	return speed;
}
