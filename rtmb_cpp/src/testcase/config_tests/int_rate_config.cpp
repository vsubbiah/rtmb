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
 
#include <list>
#include <pthread.h>
#include <stdlib.h>
#include <string.h>
#include <iomanip>

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

#include <config_test.h>
#include "TestResultsUtils.h"

void *_int_rate_test_impl(void *);

/*
 * Name         : int_rate_test()
 * Desc         : Wrapper function for integral rates configuration test
 * Args         : clParams      - Cmd line parameters.
 *                testInfo      - Holds Information about the test
 *                rtValues      - Real time values to measure the results against.
 *                resultTable   - Table to store the measured results.
 * Returns      : true on pass and false on fail.
 */

extern "C" bool int_rate_test(CmdLineParams &clParams,
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
   TestResults *tResult = new TestResults(IRATE_CONFIG, newIters);

   /* Now ready the arguments to RT thread that will run the test */
   RTThreadArgs thr_args(clParams, newIters, testInfo.getThreshold(), tResult,
         rtValues, getClockAccuracy(resultTable));

   if (clParams.isVerbose() && (clParams.getVerboseLevel() >= 1)) {
      cout << endl << "Test Report for int_rate_test test:" << endl;
      cout << "============================================" << endl;

      cout << endl << "int_rate_test: Total number of iterations: " << newIters
            << endl;
   }

   /* Create pthread attribute for the RT test thread */
   attr = (pthread_attr_t *) malloc(sizeof(pthread_attr_t));

   if (attr == NULL) {
      abort();
   }

   pthread_attr_init(attr);
   PthreadWrapper::setPthreadAttrSchedParam(attr, SCHED_FIFO,
         (PthreadWrapper::schedGetPriorityMax(SCHED_FIFO) - 10));

   /* Create and start RT test thread */
   if (pthread_create(&tid, NULL, _int_rate_test_impl, (void *) &thr_args)
         == -1) {
      perror("int_rate_test");
      cerr << "ERROR: Thread creation failed in " << "int_rate_test()" << endl;
      rc = false;
      abort();
   } else {
      pthread_join(tid, (void **) NULL);

      tResult = thr_args.tResult;
      listOfResults->push_back(tResult);
      resultTable.addResult(testInfo, listOfResults);
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
 * Name         : _int_rate_test_impl()
 * Desc         : Implements the integral rates configuration test
 *                as a separate RT thread.
 * Args         : args - Contains the test parameters.
 * Returns      : (void *) NULL.
 */
void *_int_rate_test_impl(void *args) {
   RTThreadArgs *threadArgs = (RTThreadArgs *) args;
   TestResults *tResult = threadArgs->tResult;

   int loop = 0, count = 0;
   bool done(false);
   int num = INTRATE_CONFIG_NUM;
   struct timespec startTime, endTime;
   long long timeDiff;
   bool print_flag(false);

   while (!done) {
      for (loop = 0; loop < threadArgs->iterations; loop++) {
         TimeOperations::getCurTime(&startTime);

         if (MathOperations::doSimpleIntOpern(num) == (long long) -1) {
            threadArgs->ret = false;
            return (void *) NULL;
         }

         TimeOperations::getCurTime(&endTime);
         timeDiff = TimeOperations::getTimeDiff(endTime, startTime);
         if (threadArgs->clParams.getVerboseLevel() >= 2) {
            cout << "int_rate_test: Difference "
               "between end and start times = " << timeDiff << endl;
         }
         /* Add this to results */
         tResult->setValuesAtNextIndex(timeDiff);
      }

      if (tResult -> getMedian() > threadArgs -> clockAccuracy) {
         print_flag = true;
         if (tResult->checkStdDev(threadArgs->rangePct,
               threadArgs->acceptablePct,
               threadArgs->clParams.getVerboseLevel(), false)) {
            done = true;
            break;
         } else {
            if (++count == threadArgs->threshold) {
               done = true;
               break;
            }
            if (print_flag) {
               VerbosePrint::rtmbPrintRetryMsg(threadArgs->clParams, 1,
                     "int_rate");
            }
         }
      }

      delete tResult;
      tResult = new TestResults(IRATE_CONFIG, threadArgs->iterations);
      threadArgs->tResult = tResult;
      num = num * 2;
   }

   tResult->setWorkQuantum(num);
   return (void *) NULL;
}
