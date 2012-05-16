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
#include <DiskIOReadUtils.h>

#include <config_test.h>
#include "TestResultsUtils.h"

void *_disk_io_read_test_impl(void *);

/*
 * Name         : disk_io_read_test()
 * Desc         : Wrapper function for disk IO read configuraiton test
 * Args         : clParams      - Cmd line parameters.
 *                testInfo      - Holds Information about the test
 *                rtValues      - Real time values to measure the results against.
 *                resultTable   - Table to store the measured results.
 * Returns      : true on pass and false on fail.
 */

extern "C" bool disk_io_read_test(CmdLineParams &clParams,
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
   TestResults *tResult = new TestResults(DISKIOREAD_CONFIG, new_iters);

   /* Now ready the arguments to RT thread that will run the test */
   RTThreadArgs thr_args(clParams, new_iters, testInfo.getThreshold(), tResult,
         rtValues, getClockAccuracy(resultTable));

   if (clParams.isVerbose() && (clParams.getVerboseLevel() >= 1)) {
      cout << endl << "Test Report for disk_io_read_test test:" << endl;
      cout << "======================================" << endl;
      cout << endl << "disk_io_read_test: Total number of iterations: "
            << new_iters << endl;
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
   if (pthread_create(&tid, NULL, _disk_io_read_test_impl, (void *) &thr_args)
         == -1) {
      perror("disk_io_read_test");
      cerr << "ERROR: Thread creation failed in disk_io_read_test()\n" << endl;
      rc = false;
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
 * Name         : _disk_io_read_test_impl()
 * Desc         : Implements the disk IO read configuration test
 *                as a separate RT thread.
 * Args         : args - Contains the test parameters.
 * Returns      : (void *) NULL.
 */
void *_disk_io_read_test_impl(void *args) {
   RTThreadArgs *threadArgs = (RTThreadArgs *) args;
   TestResults *tResult = threadArgs->tResult;

   int i = 0, count = 0;
   bool done(false);
   struct timespec startTime, endTime;
   long long timeDiff;
   int num = DiskIOReadUtils::getBlockSize();
   int subIters = DEFAULT_SUBITERS;
   bool print_flag(false);

   while (!done) {
      DiskIOReadUtils *readUtils = new DiskIOReadUtils(num, subIters);

      for (i = 0; i < threadArgs->iterations; i++) {

         readUtils -> openFileForRead();

         TimeOperations::getCurTime(&startTime);
         for (int j = 0; j < subIters; j++) {
            if (readUtils->doRead() != num) {
               /* we have a problem here*/
               abort();
            }
         }

         TimeOperations::getCurTime(&endTime);

         timeDiff = TimeOperations::getTimeDiff(endTime, startTime);

         if (threadArgs->clParams.getVerboseLevel() >= 2) {
            cout << "disk_io_read_test: " << "Difference between "
                  << "end and " << "start times = " << timeDiff << endl;
         }

         /* Add this to results */
         tResult->setValuesAtNextIndex(timeDiff);
         readUtils->closeFile();
      }

      if (tResult -> getMedian() > threadArgs -> clockAccuracy) {
         print_flag = true;

         if (tResult->checkStdDev(threadArgs->rangePct,
               threadArgs->acceptablePct, threadArgs->clParams.getVerboseLevel(), false)) {
            done = true;
            break;

         } else {
            if (++count == threadArgs->threshold) {
               done = true;
               break;
            }
         }
      }

      delete tResult;
      tResult = new TestResults(DISKIOREAD_CONFIG, threadArgs->iterations);
      threadArgs->tResult = tResult;
      subIters *= 2;

      if (print_flag) {
         VerbosePrint::rtmbPrintRetryMsg(threadArgs->clParams, 1,
               "disk_io_read");
      }

      readUtils -> cleanup();
      delete readUtils;
   }

   tResult->setWorkQuantum(subIters * num);

   return (void *) NULL;
}
