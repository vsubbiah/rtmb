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
#include <MatrixOperationsWithObjects.h>
#include <PthreadWrapper.h>
#include <stdlib.h>
#include <stdio.h>
#include <iostream>
#include <fstream>
#include <pthread.h>
#include <time.h>
using namespace std;

static int subIterations = -1;
static int iterations = -1;
static int sleepTime = -1;
static long long timeToStart = 0;
static char *name;
static int priority;
static int needLog;

void * threadRun(void * arg) {
   MatrixObjects a(10, 10), b(10, 10);
   char buf[100];
   struct timespec currTime;

   clock_gettime(CLOCK_REALTIME, &currTime);

   long long currentSec = (long long) currTime.tv_sec;
   long long currentNano = (long long) currTime.tv_nsec;

   if (currentSec < timeToStart) {
      long long second_diff = timeToStart - currentSec - 1;
      long long nano_diff = 1000000000L - currentNano;
      long long currentNanos = TimeOperations::convertTimeToNsec(currTime);
      long long waitTimeInNanos = (second_diff * 1000000000LL) + nano_diff;

      TimeOperations::doNanoSleep(waitTimeInNanos);
   }

   MatrixMultiplier mult(subIterations, iterations);
   mult.setSleepTime(sleepTime);
   mult.runIterationsWithSleepTime();

   if (needLog) {
      long long* val = mult.getDiffTime();

      ofstream fout(name);

      for (int i = 0; i < iterations; i++) {
         fout.setf(ios::dec, ios::basefield);
         fout << val[i] << endl;
      }
   }

   return 0;
}

int main(int argc, char **argv) {
   pthread_attr_t attr;
   subIterations = atoi(argv[1]);
   iterations = atoi(argv[2]);
   sleepTime = atoi(argv[3]);
   timeToStart = strtoll(argv[4], NULL, 10);
   name = argv[5];
   priority = atoi(argv[6]);
   needLog = atoi(argv[7]);

   pthread_t thrd;

   //define DEBUG_TEST only while debugging multicpu subtests
#ifdef DEBUG_TEST
   cout <<"iterations1 = " << iterations << endl;
   cout <<"subIterations = " << subIterations << endl;
   cout <<"sleep Time = " << sleepTime << endl;
   cout <<"timeToStart = " << timeToStart << endl;
   cout <<"name = " << name << endl;
   cout <<"priority = " << priority << endl;
#endif

   pthread_attr_init(&attr);

   PthreadWrapper::setPthreadAttrSchedParam(&attr, SCHED_FIFO, priority);

   pthread_create(&thrd, &attr, threadRun, NULL);
   pthread_join(thrd, NULL);
}
