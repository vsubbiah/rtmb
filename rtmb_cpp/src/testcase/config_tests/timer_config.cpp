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
 
#include <iostream>
#include <list>
#include <pthread.h>
#include <cstdlib>
#include <stdlib.h>
#include <string.h>

#include <CmdLineParams.h>
#include <InputParams.h>
#include <RTMB.h>
#include <BenchMarkTestInfoBase.h>
#include <BenchMarkTestInfo.h>
#include <TestResults.h>
#include <VerbosePrint.h>
#include <RealTimeValues.h>
#include <ResultTable.h>

#include <PthreadWrapper.h>
#include <TimeOperations.h>
#include <MathOperations.h>
#include <config_test.h>
#include "TestResultsUtils.h"

void *TimerConfigImpl(void *);

/*
 * Name         : timer_config()
 * Desc         : Wrapper function for timer configration test
 * Args         : clParams      - Cmd line parameters.
 *                testInfo      - Holds Information about the test
 *                rtValues      - Real time values to measure the results against.
 *                resultTable   - Table to store the measured results.
 * Returns      : true on pass and false on fail.
 */

extern "C" bool timer_config(CmdLineParams &clParams,
      BenchMarkTestInfo &testInfo, RealTimeValues &rtValues,
      ResultTable &resultTable) {

   /* If we are running quick mode, then we should just do 1 iteration */
   int newIters = testInfo.getIterations();
   if (clParams.isQuickRun()) {
      newIters = 1;
   } else {
      newIters = newIters * rtValues.getMultiplier();
   }
   testInfo.setIterations(newIters);

   list<TestResults *> *listOfResults = new list<TestResults *> ();
   TestResults *tResult = new TestResults(SYSTIMER_CONFIG, newIters);

   RTThreadArgs thr_args(clParams, newIters, testInfo.getThreshold(), tResult,
         rtValues, getClockAccuracy(resultTable));
   pthread_t tid;
   pthread_attr_t *attr = NULL;
   bool rc(true);

   if (clParams.isVerbose() && (clParams.getVerboseLevel() >= 1)) {
      cout << endl << "Test Report for timer_config test:" << endl;
      cout << "=========================================" << endl;
      cout << endl << "timer_config: Total number of iterations: " << newIters
            << endl;
   }

   /* Create pthread attribute for the RT test thread */
   attr = (pthread_attr_t *) malloc(sizeof(pthread_attr_t));
   if (attr != NULL) {
      pthread_attr_init(attr);
      PthreadWrapper::setPthreadAttrSchedParam(attr, SCHED_FIFO,
            (PthreadWrapper::schedGetPriorityMax(SCHED_FIFO) - 10));

      /* Create and start RT test thread */
      if (pthread_create(&tid, NULL, TimerConfigImpl, (void *) &thr_args) == -1) {
         perror("timer_config");
         cerr << "ERROR: Thread creation failed"
            " in timer_config()\n" << endl;
         rc = false;
      } else {
         pthread_join(tid, (void **) NULL);
         tResult = thr_args.tResult;
         listOfResults->push_back(tResult);

         if (clParams.isVerbose() && (clParams.getVerboseLevel() >= 1)) {
            cout << "timer_config: " << "Benchmark will be configured"
                  << " to expect the timer to be " << "accurate to "
                  << MICROSEC(tResult->getMedian()) << " us." << endl;
         }

         rc = true;

         resultTable.addResult(testInfo, listOfResults, rc);
      }
   } else {
      cerr << "ERROR: malloc() of pthread_attr_t"
         " failed in timer_config()" << endl;
      rc = false;
   }
   if (attr != NULL) {
      pthread_attr_destroy(attr);
      free(attr);
   }

   if (!rc) {
      if (tResult != NULL) {
         delete tResult;
      }
   }
   return rc;
}

/*
 * Name         : _timer_config_impl()
 * Desc         : Implements the timer configuration test
 *                as a separate RT thread.
 * Args         : args - Contains the test parameters.
 * Returns      : (void *) NULL.
 */
void runTimerTest(int num, RTThreadArgs* threadArgs) {
   TestResults *tResult = threadArgs->tResult;

   int loop = 0, count = 0;
   bool done(false);

   struct timespec startTime, endTime;
   long long timeDiff;
   struct timespec req_time;

   req_time.tv_sec = num / 1000000000LL;
   req_time.tv_nsec = num % 1000000000LL;

   for (loop = 0; loop < threadArgs->iterations; loop++) {
      TimeOperations::getCurTime(&startTime);

      if (nanosleep(&req_time, NULL) == (long long) -1) {
         abort();
      }

      TimeOperations::getCurTime(&endTime);

      timeDiff = TimeOperations::getTimeDiff(endTime, startTime);
      if ((threadArgs->clParams.getVerboseLevel() >= 2)) {
         cout << "timer_config: Difference between end and "
               << "start times = " << timeDiff << endl;
      }

      /* Add this to results */
      tResult->setValuesAtNextIndex(timeDiff);
   }

   tResult ->setWorkQuantum(num);
}

void *TimerConfigImpl(void *args) {
   RTThreadArgs *threadArgs = (RTThreadArgs *) args;
   TestResults *tResult = threadArgs->tResult;
   int count = 0;
   bool done(false);
   bool testStatus(false);
   int num = NANOSLEEP_TIME;
   int previousQuantum;
   bool print_flag(false);
   long long clockAccuracy = threadArgs -> clockAccuracy;

   if (num > clockAccuracy / 2) {
      num = clockAccuracy / 2;

      if (num == 0) {
         num = 1;
      }
   }

   testStatus = false;
   while (!done) {
      runTimerTest(num, threadArgs);

      if (tResult -> getMedian() >= clockAccuracy) {
         print_flag = true;
         if (tResult->checkStdDev(threadArgs->rangePct,
               threadArgs->acceptablePct,
               threadArgs->clParams.getVerboseLevel(), false)) {
            testStatus = true;
            done = true;
            break;
         }

         if (++count == threadArgs->threshold) {
            done = true;
            break;
         }
         previousQuantum = num;
      }

      num = num * 2;
      delete tResult;
      tResult = new TestResults(SYSTIMER_CONFIG, threadArgs->iterations);
      threadArgs->tResult = tResult;

      if (print_flag) {
         VerbosePrint::rtmbPrintRetryMsg(threadArgs->clParams, 1,
               "timer_config");
      }
   }

   if (testStatus) {
      TestResults *passedResult = new TestResults(SYSTIMER_CONFIG,
            threadArgs->iterations);

      passedResult ->cloneValues(threadArgs->tResult);

#ifdef TRACE_MSG
      cout << "work quantum of passed result = "
      << passedResult -> getWorkQuantum() << endl;
#endif
      int startQuantum = previousQuantum;
      int lastQuantum = num;
      int midQuantum = 0;

      while (startQuantum <= lastQuantum) {
         threadArgs -> tResult -> removeOldValues();
         midQuantum = (startQuantum + lastQuantum) / 2;

#ifdef TRACE_MSG
         cout << " start = " << startQuantum;
         cout << " middle = " << midQuantum;
         cout << " end = " << lastQuantum << endl;
#endif

         runTimerTest(midQuantum, threadArgs);

         if (tResult->checkStdDev(threadArgs->rangePct,
               threadArgs->acceptablePct,
               threadArgs->clParams.getVerboseLevel(), false)
               && tResult->getMedian() > clockAccuracy) {
            testStatus = true;
            /* test passed */
            lastQuantum = midQuantum - 1;
            passedResult = new TestResults(SYSTIMER_CONFIG,
                  threadArgs->iterations);
            passedResult ->cloneValues(threadArgs->tResult);
#ifdef TRACE_MSG
            cout << "work Quantum passedResult="
            << passedResult -> getWorkQuantum()
            << endl;
#endif
         } else {
            startQuantum = midQuantum + 1;
         }
      }

      delete threadArgs -> tResult;
      threadArgs -> tResult = passedResult;
   }
#ifdef TRACE_MSG
   cout << "work Quantum final ="
   << threadArgs ->tResult -> getWorkQuantum() << endl;
#endif
   return (void *) NULL;
}
