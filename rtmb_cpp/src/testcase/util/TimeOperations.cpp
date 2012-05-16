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
 
#include <TimeOperations.h>
#include <stdio.h>
#include <errno.h>

/* Name    : convertTimeToNsec()
 * Desc    : Converts time vals in timespec to nano secs.
 * Args    : ts - timespec.
 * Return  : time_ns - time in nano secs.
 */
long long TimeOperations::convertTimeToNsec(struct timespec ts) {
   long long timeNs = (long long) ts.tv_sec * 1000000000LL;
   long long nsec = (long long) ts.tv_nsec;
   timeNs = (long long) timeNs + nsec;
   return timeNs;
}

/* Name    : getCurTime()
 * Desc    : Gets current time.
 * Args    : ts - timespec.
 * Return  : 0 on success and -1 on error.
 */
void TimeOperations::getCurTime(struct timespec *time) {
   clock_gettime(CLOCK_MONOTONIC, time);
}

/* Name    : getTimeDiff()
 * Desc    : Gets difference between end and start times.
 * Args    : start_time - timespec having start time.
 *           end_time   - timespec having end time.
 * Return  : time_diff - time difference in nano secs.
 */
long long TimeOperations::getTimeDiff(struct timespec endTime,
      struct timespec startTime) {
   long long timeDiff;
   timeDiff = convertTimeToNsec(endTime) - convertTimeToNsec(startTime);
   return timeDiff;
}

/* Name    : doNanoSleep()
 * Desc    : sleep()s for 'nsecs' nano seconds.
 * Args    : nsecs - nano seconds to sleep for.
 * Return  : true on success and false on error.
 */
bool TimeOperations::doNanoSleep(int nsecs) {
   return doNanoSleep((long long) nsecs);
}

bool TimeOperations::doNanoSleep(long long nsecs) {
   struct timespec rqtp, rmtp;

   /*
    * Make sure that we did not get a value greater than or
    * equal to 1 sec in 'nsecs' argument of this function.
    */

   int secs = nsecs / 1000000000LL;
   if (secs > 0) {

      /* nsecs is greater than 1 sec.
       * split it into sec and nsec
       * as required.
       */
      rqtp.tv_sec = secs;
      rqtp.tv_nsec = nsecs % 1000000000LL;
   } else {
      rqtp.tv_sec = 0;
      rqtp.tv_nsec = (long) nsecs;
   }

   /*
    * This loop implements a retry logic for the sleep.
    * Problem is, if we do not retry and were interrupted
    * very early into the sleep, it may skew the results.
    * On the contrary, if we do retry once interrupted, we
    * may sleep for slightly longer than expected. Is
    * retry not less severe is of these 2 ?
    */
   while (nanosleep(&rqtp, &rmtp) != 0) {
      switch (errno) {
      case EINTR:
         rqtp.tv_sec = rmtp.tv_sec;
         rqtp.tv_nsec = rmtp.tv_nsec;
         break;
      case EINVAL:
      default:
         perror("doNanoSleep");
         return false;
      }
   }
   return true;
}
