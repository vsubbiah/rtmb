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
#include <stdio.h>
#include <cstdlib>
#include <stdlib.h>
#include <string.h>

#include <CmdLineParams.h>
#include <InputParams.h>
#include <RTMB.h>
#include <BenchMarkTestInfo.h>
#include <TestResults.h>
#include <VerbosePrint.h>
#include <RealTimeValues.h>
#include <ResultTable.h>
#include <PthreadWrapper.h>
#include <TimeOperations.h>
#include <MathOperations.h>
#include "TestResultsUtils.h"

#include <config_test.h>


bool runClockTest(int num, RTThreadArgs *threadArgs) {
   struct timespec startTime, endTime;
   int loop;
   long long timeDiff;
   TestResults *tResult = threadArgs->tResult;
   bool done(false);
   for (loop = 0; loop < threadArgs->iterations; loop++) {
      TimeOperations::getCurTime(&startTime);
      TimeOperations::getCurTime(&endTime);
      timeDiff = TimeOperations::getTimeDiff(endTime, startTime);
      if (threadArgs->clParams.getVerboseLevel() >= 2) {
         cout << "clock_config: Difference between end and "
               << "start times = " << timeDiff << endl;
      }

      /* Add this to results */
      tResult->setValuesAtNextIndex(timeDiff);
   }

   if (tResult->checkStdDev(threadArgs->rangePct, threadArgs->acceptablePct,
         threadArgs->clParams.getVerboseLevel(), false)) {
      done = true;
   } else {
      done = false;
   }

   return done;
}

/*
 * Name         : ClockConfigImpl()
 * Desc         : Implements the clock configuration test
 *                as a separate RT thread.
 * Args         : args - Contains the test parameters.
 * Returns      : (void *) NULL.
 */
void *ClockConfigImpl(void *args) {
   RTThreadArgs *threadArgs = (RTThreadArgs *) args;
   TestResults *tResult = threadArgs->tResult;

   int loop = 0, count = 0;
   bool done(false);
   int previousQuantum = 0;
   int num = DEF_SZ_CLK_CONF;
   struct timespec startTime, endTime;
   long long timeDiff;
   bool testStatus;

   testStatus = runClockTest(num, threadArgs);

   tResult -> setWorkQuantum(num);
   threadArgs ->ret = true;
   return (void *) NULL;
}

/*
 * Name         : clock_config()
 * Desc         : Wrapper function for clock configuration test
 * Args         : clParams      - Cmd line parameters.
 *                testInfo      - Holds Information about the test
 *                rtValues      - Real time values to measure the results
 *                against.
 *                resultTable   - Table to store the measured results.
 * Returns      : true on pass and false on fail.
 */

extern "C" bool clock_config(CmdLineParams &clParams,
      BenchMarkTestInfo &testInfo, RealTimeValues &rtValues,
      ResultTable &resultTable) {

   pthread_t tid;
   pthread_attr_t *attr = NULL;
   int newIters = testInfo.getIterations();
   bool rc(true);

   /* If we are running quick mode, then we should just do 1 iteration */
   if (clParams.isQuickRun()) {
      newIters = 1;
   } else {
      newIters = newIters * rtValues.getMultiplier();
   }
   testInfo.setIterations(newIters);

   list<TestResults *> *listOfResults = new list<TestResults *> ();
   TestResults *tResult = new TestResults(SYSCLOCK_CONFIG, newIters);

   /* Now ready the arguments to RT thread that will run the test */
   RTThreadArgs thr_args(clParams, newIters, testInfo.getThreshold(), tResult,
         rtValues, getClockAccuracy(resultTable));

   /* Create pthread attribute for the RT test thread */
   attr = (pthread_attr_t *) malloc(sizeof(pthread_attr_t));

   if (attr == NULL) {
      abort();
   }

   if (clParams.isVerbose() && (clParams.getVerboseLevel() >= 1)) {
      cout << endl << "Test Report for clock_config test:" << endl;
      cout << "=========================================" << endl;

      cout << endl << "clock_config: Total number of iterations: " << newIters
            << endl;
   }

   pthread_attr_init(attr);
   PthreadWrapper::setPthreadAttrSchedParam(attr, SCHED_FIFO,
         (PthreadWrapper::schedGetPriorityMax(SCHED_FIFO) - 10));

   /* Create and start RT test thread */
   if (pthread_create(&tid, NULL, ClockConfigImpl, (void *) &thr_args) == -1) {
      perror("clock_config");
      cerr << "ERROR: Thread creation failed in clock_config()" << endl;
      rc = false;
   } else {
      pthread_join(tid, (void **) NULL);
      tResult = thr_args.tResult;
      listOfResults->push_back(tResult);
      resultTable.addResult(testInfo, listOfResults);

      if (clParams.isVerbose() && (clParams.getVerboseLevel() >= 1)) {
         cout << "clock_config: " << "Benchmark will be configured"
               << " to expect the clock monotonicity to be " << "accurate to "
               << MICROSEC(tResult->getMedian()) << " us." << endl;
      }
   }

   assert(getClockAccuracy( resultTable )> 0);
   if (attr != NULL) {
      pthread_attr_destroy(attr);
      free(attr);
   }

   return rc;
}
