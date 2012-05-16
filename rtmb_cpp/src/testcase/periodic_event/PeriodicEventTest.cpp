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
 
#include <PeriodicEventTest.h>
#include <stdio.h>
#include <time.h>
#include <signal.h>
#include <unistd.h>
#include <stdlib.h>
#include <errno.h>

static pthread_attr_t attr;
static struct timespec startTime;
static struct timespec period;
static int eventsToFire;
static int eventsFired;
static struct timespec *dispatchTimes;
volatile bool done(false);
static timer_t timerId;
static sigset_t sigSet;
static struct itimerspec fire_time;
static struct sigevent evp;
static struct sigaction sigAct;

bool doRun(TestResults *result, CmdLineParams &clParams, int majorIterations,
      int subIters, long long timerAccuracy);
bool addResults(TestResults *results, RealTimeValues &rtValues,
      CmdLineParams &clParams, int iterations, ResultTable& resultTable);
bool checkResults(TestResults *results, RealTimeValues &rtValues,
      CmdLineParams &clParams, int iterations, ResultTable& resultTable);
void timerExpiryHandler(int signal);

/*
 * Name  : periodic_event()
 * Desc  : Wrapper function for disk IO read configuraiton test
 * Args  : clParams      - Cmd line parameters.
 *      testInfo      - Holds Information about the test
 *      rtValues      - Real time values to measure the results against.
 *      resultTable   - Table to store the measured results.
 * Returns      : true on pass and false on fail.
 */
extern "C" bool periodic_event(CmdLineParams &clParams,
      BenchMarkTestInfo &testInfo, RealTimeValues &rtValues,
      ResultTable &resultTable) {
   pthread_attr_t attr;
   int newIters = testInfo.getIterations();
   bool rc(false);
   bool runStatus(false);

   /* If we are running quick mode, then we should just do 1 iteration */
   if (clParams.isQuickRun()) {
      newIters = 1;
   } else {
      newIters = newIters * rtValues.getMultiplier();
   }
   testInfo.setIterations(newIters);

   if (clParams.isVerbose() && (clParams.getVerboseLevel() >= 1)) {
      cout << endl << "Test Report for periodic_event test:" << endl;
      cout << "====================================" << endl;
      cout << endl << "periodic_event: Total number of iterations: "
            << newIters << endl;
   }
   list<TestResults *> *listOfResults = new list<TestResults *> ();
   TestResults *tResults = NULL;
   long long timerAccuracy = getTimeAccuracy(resultTable);

   for (int subIters = 1; subIters < testInfo._threshold; subIters++) {
      if (tResults != NULL) {
         delete (tResults);
      }
      if (dispatchTimes != NULL) {
         free(dispatchTimes);
      }
      tResults = new TestResults(PERIODIC_EVENTS_TEST, newIters);
      tResults -> setTestResultName("Periodic event test");
      runStatus = doRun(tResults, clParams, newIters, subIters, timerAccuracy);
      if (runStatus) {
         if (addResults(tResults, rtValues, clParams, newIters, resultTable)) {
            rc = checkResults(tResults, rtValues, clParams, newIters,
                  resultTable);
            if (rc) {
               break;
            } else {
               VerbosePrint::rtmbVerbosePrintlnOut(clParams, 1,
                     "periodic_event: Retrying with a larger "
                        " time interval...");
               continue;
            }
         }
      } else {
         VerbosePrint::rtmbVerbosePrintlnOut(clParams, 1,
               "periodic_event: Retrying test with a larger "
                  " time interval due to internal errors...");
         rc = runStatus;
         continue;
      }
   }

   if (runStatus == true) {
      listOfResults->push_back(tResults);
      list<TestScore *> *tScore = generateScores(listOfResults,
            testInfo.getReferenceScore());
      FinalBenchMarkTestResult *fbmt = new FinalBenchMarkTestResult(tScore,
            true);
      resultTable.addResult(testInfo, fbmt);
   } else {
      /* functional failure */
      abort();
   }

   return runStatus;
}

bool addResults(TestResults *results, RealTimeValues &rtValues,
      CmdLineParams &clParams, int iterations, ResultTable& resultTable) {
   long long per = TimeOperations::convertTimeToNsec(period);
   long long expectedFireTime = TimeOperations::convertTimeToNsec(startTime);
   for (int i = 0; i < iterations; i++) {
      long long timeDiff = TimeOperations::convertTimeToNsec(dispatchTimes[i])
            - expectedFireTime;
      VerbosePrint::rtmbVerbosePrintlnOut(clParams, 2,
            "periodic_event: Difference between actual and "
               "expected event fire time is %.3f us", MICROSEC(timeDiff));
      expectedFireTime += per;
      results -> setValuesAtNextIndex(timeDiff);
   }

   return true;
}

bool checkResults(TestResults *results, RealTimeValues &rtValues,
      CmdLineParams &clParams, int iterations, ResultTable& resultTable) {

   if (results -> checkStdDev(rtValues.getRangePercentage(),
         rtValues.getAcceptablePercentage(),
         clParams.getVerboseLevel(), 0)) {
      return true;
   } else {
      return false;
   }
}

bool doRun(TestResults *result, CmdLineParams &clParams, int iterations,
      int subIters, long long timerAccuracy) {
   int rc;
   dispatchTimes = (struct timespec *) calloc(iterations,
         sizeof(struct timespec));
   if (dispatchTimes == NULL) {
      cerr << "ERROR: calloc() failed" << endl;
      abort();
   }

   /* Set up timer: */
   memset(&evp, 0, sizeof(evp));

   evp.sigev_notify = SIGEV_SIGNAL;
   evp.sigev_signo = SIGUSR1;
   evp.sigev_value.sival_int = 0;

   rc = sigemptyset(&sigSet);
   if (rc != 0) {
      VerbosePrint::rtmbVerbosePrintlnOut(clParams, 1,
            "ERROR: during sigemptyset");
      return ERROR;
   }
   sigAct.sa_handler = (void(*)(int)) timerExpiryHandler;
   sigAct.sa_flags = 0;

   rc = sigemptyset(&sigAct.sa_mask);
   if (rc != 0) {
      VerbosePrint::rtmbVerbosePrintlnOut(clParams, 1,
            "ERROR: during sigemptyset1");
      return ERROR;
   }

   rc = sigaction(evp.sigev_signo, &sigAct, 0);
   if (rc != 0) {
      VerbosePrint::rtmbVerbosePrintlnOut(clParams, 1,
            "ERROR: during sigemptyset1 %d\n", errno);
      return ERROR;
   }

   rc = clock_gettime(CLOCK_MONOTONIC, &startTime);
   if (rc != 0) {
      VerbosePrint::rtmbVerbosePrintlnOut(clParams, 1,
            "ERROR: during timer_gettime");
      return false;
   }

   rc = timer_create(CLOCK_MONOTONIC, &evp, &timerId);
   if (rc != 0) {
      VerbosePrint::rtmbVerbosePrintlnOut(clParams, 1,
            "ERROR: during timer_create");
      return false;
   }

   startTime.tv_sec += 5;
   period.tv_sec = 0;
   period.tv_nsec = subIters * timerAccuracy;
   eventsToFire = iterations;
   eventsFired = 0;
   done = 0;

   fire_time.it_value.tv_sec = startTime.tv_sec;
   fire_time.it_value.tv_nsec = startTime.tv_nsec;
   fire_time.it_interval.tv_sec = period.tv_sec;
   fire_time.it_interval.tv_nsec = period.tv_nsec;
   rc = timer_settime(timerId, TIMER_ABSTIME, &fire_time, NULL);
   if (rc != 0) {
      VerbosePrint::rtmbVerbosePrintlnOut(clParams, 1,
            "ERROR: during timer_settime");
      return false;
   }
   while (!done) {
      struct timespec timeout;
      timeout.tv_sec = 30;
      timeout.tv_nsec = 0;
      sigtimedwait(&sigSet, NULL, &timeout);
   }

   return true;
}

void timerExpiryHandler(int signo) {
   struct timespec firedTime;

   if (clock_gettime(CLOCK_MONOTONIC, &firedTime) < 0) {
      perror("clock_gettime:");
      abort();
   }

   if (signo == SIGUSR1 && !done) {
      if (eventsFired < eventsToFire) {
         dispatchTimes[eventsFired] = firedTime;
         eventsFired += 1;
      } else {
         done = 1;
         timer_delete(timerId);
      }
   }
}
