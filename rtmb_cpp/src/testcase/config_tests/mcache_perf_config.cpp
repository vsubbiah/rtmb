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
#include <BenchMarkTestInfo.h>
#include <TestResults.h>
#include <VerbosePrint.h>
#include <RealTimeValues.h>
#include <ResultTable.h>

#include <PthreadWrapper.h>
#include <TimeOperations.h>
#include <ArrayOperations.h>

#include <config_test.h>
#include "TestResultsUtils.h"

void *_mem_cache_test_impl(void *);

/*
 * Name         : mem_cache_test()
 * Desc         : Wrapper function for memory cache performance configuration test
 * Args         : clParams      - Cmd line parameters.
 *                testInfo      - Holds Information about the test
 *                rtValues      - Real time values to measure the results against.
 *                resultTable   - Table to store the measured results.
 * Returns      : true on pass and false on fail.
 */

extern "C" bool mem_cache_test(CmdLineParams &clParams,
      BenchMarkTestInfo &testInfo, RealTimeValues &rtValues,
      ResultTable &resultTable) {

   pthread_t tid;
   pthread_attr_t *attr = NULL;
   int new_iters = testInfo.getIterations();
   bool rc(true);

   /* If we are running quick mode, then we should just do 1 iteration */
   if (clParams.isQuickRun()) {
      new_iters = 1;
   } else {
      new_iters = new_iters * rtValues.getMultiplier();
   }
   testInfo.setIterations(new_iters);

   list<TestResults *> *listOfResults = new list<TestResults *> ();
   TestResults *tResult = new TestResults(MCACHE_CONFIG, new_iters);

   /* Now ready the arguments to RT thread that will run the test */
   RTThreadArgs thr_args(clParams, new_iters, testInfo.getThreshold(), tResult,
         rtValues, getClockAccuracy(resultTable));

   if (clParams.isVerbose() && (clParams.getVerboseLevel() >= 1)) {
      cout << endl << "Test Report for mem_cache_test test:" << endl;
      cout << "=============================================" << endl;
      cout << endl << "mem_cache_test: Total number of iterations: "
            << new_iters << endl;
   }

   /* Create pthread attribute for the RT test thread */
   attr = (pthread_attr_t *) malloc(sizeof(pthread_attr_t));
   if (attr != NULL) {
      pthread_attr_init(attr);
      PthreadWrapper::setPthreadAttrSchedParam(attr, SCHED_FIFO,
            (PthreadWrapper::schedGetPriorityMax(SCHED_FIFO) - 10));

      /* Create and start RT test thread */
      if (pthread_create(&tid, NULL, _mem_cache_test_impl, (void *) &thr_args)
            == -1) {
         perror("mem_cache_test");
         cerr << "ERROR: Thread creation failed in " << "mem_cache_test()"
               << endl;
         rc = false;
      } else {
         pthread_join(tid, (void **) NULL);
         tResult = thr_args.tResult;
         listOfResults->push_back(tResult);

         if (thr_args.ret) {
            if (clParams.isVerbose()) {
               if (clParams.getVerboseLevel() >= 1) {
                  cout << "mem_cache_test: " << "Benchmark will be configured"
                        << " to expect the memory cache "
                        << " performance of this system "
                        << " to be accurate to " << tResult->getMedian()
                        / 1000.0 << " us." << endl;
               }
            }

            rc = true;

         } else {
            rc = false;
         }

         resultTable.addResult(testInfo, listOfResults, rc);
      }
   } else {
      cerr << "ERROR: malloc() of pthread_attr_t failed in "
            << "mem_cache_test()" << endl;
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
 * Name         : _mem_cache_test_impl()
 * Desc         : Implements the memory cache performance config test
 *                as a separate RT thread.
 * Args         : args - Contains the test parameters.
 * Returns      : (void *) NULL.
 */
void *_mem_cache_test_impl(void *args) {
   RTThreadArgs *threadArgs = (RTThreadArgs *) args;
   TestResults *tResult = threadArgs->tResult;

   int loop = 0, count = 0;
   bool done(false);
   int num = MCACHE_CONFIG_ARR_SIZE;
   struct timespec startTime, endTime;
   long long timeDiff;
   bool printFlag(false);

   while (!done) {
      for (loop = 0; loop < threadArgs->iterations; loop++) {
         TimeOperations::getCurTime(&startTime);

         ArrayOperations::doArrayOpern(num);

         TimeOperations::getCurTime(&endTime);
         timeDiff = TimeOperations::getTimeDiff(endTime, startTime);

         if (threadArgs->clParams.getVerboseLevel() >= 2) {
            cout << "mem_cache_test: Difference "
               "between end and start times = " << MICROSEC(timeDiff) << " us"
                  << endl;
         }

         /* Add this to results */
         tResult->setValuesAtNextIndex(timeDiff);
      }

      if (tResult->getMedian() > threadArgs -> clockAccuracy) {
         printFlag = true;
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
            if (printFlag) {
               VerbosePrint::rtmbPrintRetryMsg(threadArgs->clParams, 1,
                     "mcache_perf");
            }
         }
      }

      delete tResult;
      tResult = new TestResults(MCACHE_CONFIG, threadArgs->iterations);
      threadArgs->tResult = tResult;
      num = num * 2;
   }

   tResult->setWorkQuantum(num);
   return (void *) NULL;
}
