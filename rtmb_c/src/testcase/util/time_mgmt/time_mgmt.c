/*******************************************************************************
 * Copyright (c) 2008 International Business Machines Corp.
 * All rights reserved. This program and the accompanying materials
 * are made available under the terms of the Eclipse Public License v1.0
 * which accompanies this distribution, and is available at
 * http://www.eclipse.org/legal/epl-v10.html
 *
 * Contributors:
 *    Ajith Ramanath            - initial API and implementation
 *    Radhakrishnan Thangamuthu - initial API and implementation
 *    Mike Fulton               - initial API and implementation
 *******************************************************************************/
 
#include <time_mgmt.h>

/* Name    : convert_time_to_nsec()
 * Desc    : Converts time vals in timespec to nano secs.
 * Args    : ts - timespec.
 * Return  : time_ns - time in nano secs.
 */
long long convert_time_to_nsec(struct timespec ts) {
	long long time_ns = (long long) ts.tv_sec * 1000000000LL;
	long long nsec = (long long) ts.tv_nsec;
	time_ns = (long long) time_ns + nsec;
	return time_ns;
}

/* Name    : get_cur_time()
 * Desc    : Gets current time.
 * Args    : ts - timespec.
 * Return  : 0 on success and -1 on error.
 */
int get_cur_time(struct timespec *time) {
	if (clock_gettime(CLOCK_MONOTONIC, time) == SUCCESS) {
		return SUCCESS;
	} else {
		perror("get_cur_time(): ");
		return ERROR;
	}
}

/* Name    : get_time_diff()
 * Desc    : Gets difference between end and start times.
 * Args    : start_time - timespec having start time.
 *	     end_time   - timespec having end time.
 * Return  : time_diff - time difference in nano secs.
 */
long long get_time_diff(struct timespec start_time, struct timespec end_time) {
	long long time_diff;
	time_diff = convert_time_to_nsec(end_time) - convert_time_to_nsec(
	        start_time);
	return time_diff;
}

/* Name    : do_nano_sleep()
 * Desc    : sleep()s for 'nsecs' nano seconds.
 * Args    : nsecs - nano seconds to sleep for.
 * Return  : 0 on success and -1 on error.
 */
int do_nano_sleep(int nsecs) {
	struct timespec rqtp, rmtp;

	/*
	 * Make sure that we did not get a value greater than or
	 * equal to 1 sec in 'nsecs' argument of this function.
	 */

	int secs = nsecs / 1000000000;
	if (secs > 0) {

		/* nsecs is greater than 1 sec.
		 * split it into sec and nsec
		 * as required.
		 */
		rqtp.tv_sec = secs;
		rqtp.tv_nsec = nsecs % 1000000000;
	} else {
		rqtp.tv_sec = 0;
		rqtp.tv_nsec = nsecs;
	}

	/*
	 * This loop implements a retry logic for the sleep.
	 * Problem is, if we do not retry and were interrupted
	 * very early into the sleep, it may skew the results.
	 * On the contrary, if we do retry once interrupted, we
	 * may sleep for slightly longer than expected. Is
	 * retry not less severe is of these 2 ?
	 */
	while (nanosleep(&rqtp, &rmtp) != SUCCESS) {
		switch (errno) {
		case EINTR:
			rqtp.tv_sec = rmtp.tv_sec;
			rqtp.tv_nsec = rmtp.tv_nsec;
			break;
		case EINVAL:
		default:
			perror("do_nano_sleep:");
			return ERROR;
		}
	}
	return SUCCESS;
}
