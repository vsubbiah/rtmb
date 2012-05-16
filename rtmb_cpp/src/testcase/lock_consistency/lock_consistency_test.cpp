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
#include <assert.h>

#include <CmdLineParamsBase.h>
#include <CmdLineParams.h>
#include <InputParams.h>
#include <RTMB.h>
#include <BenchMarkTestInfo.h>
#include <TestResults.h>
#include <TestResultsUtils.h>
#include <RealTimeValues.h>
#include <ResultTable.h>

#include <PthreadWrapper.h>
#include <TimeOperations.h>
#include <DiskIOReadUtils.h>
#include <VerbosePrint.h>
#include <MathOperations.h>
#include <errno.h>
#include <TestResultsUtils.h>
#include <lock_consistency_test.h>

/*
 * Name     : lockConsistency()
 * Desc     : Tests if we are able to lock and unlock consistently
 * Args     : args - Test arguments passed from harness.
 * Returns  : NULL.
 */
extern "C" bool lock_consistency(CmdLineParams &clParams,
      BenchMarkTestInfo &testInfo, RealTimeValues &rtValues,
      ResultTable &resultTable) {
   pthread_t tid;
   pthread_attr_t *attr = NULL;
   int newIters = testInfo.getIterations();
   bool rc(true);
   pthread_t highPriorityThrd;
   int i = 0, j = 0;
   pthread_attr_t highPrioAttr;
   pthread_mutex_t mutex;

   /* If we are running quick mode, then we should just do 1 iteration */
   if (clParams.isQuickRun()) {
      newIters = 1;
   } else {
      newIters = newIters * rtValues.getMultiplier();
   }
   testInfo.setIterations(newIters);

   /* Now ready the arguments to RT thread that will run the test */
   bool verbose = clParams.isVerbose();
   int verboseLvl = clParams.getVerboseLevel();
   int threshold = testInfo.getThreshold();
   double rangePct = rtValues.getRangePercentage();
   double acceptablePct = rtValues.getAcceptablePercentage();
   long long clockAccuracy = getClockAccuracy(resultTable);

   list<TestResults *> *listOfResults = new list<TestResults *> ();
   TestResults* tResult = new TestResults(LOCK_CONSISTENCY_TEST, newIters);

   if (clParams.isVerbose() && (clParams.getVerboseLevel() >= 1)) {
      cout << endl << "Test Report for lock_consistency test:" << endl;
      cout << "===============================================" << endl;
      cout << endl << "lock_consistency: Total number of iterations: "
            << newIters << endl;
   }

   bool done(false);
   bool printFlag(false);
   int subIters = 1;
   struct timespec startTime, endTime;
   long long diffTime;
   int count = 0;
   while (!done) {
      tResult -> removeOldValues();
      subIters *= 2;
      pthread_mutex_init(&mutex, NULL);

      for (int i = 0; i < newIters; i++) {
         TimeOperations::getCurTime(&startTime);

         for (j = 0; j < subIters; j++) {
            pthread_mutex_lock(&mutex);
            pthread_mutex_unlock(&mutex);
         }

         TimeOperations::getCurTime(&endTime);

         diffTime = TimeOperations::getTimeDiff(endTime, startTime);
         VerbosePrint::rtmbVerbosePrintlnOut(clParams, 2,
               "lock_consistency: Difference between "
                  "end and start times = %.3f us", MICROSEC(diffTime));
         tResult -> setValuesAtNextIndex(diffTime);
      }

      if (tResult -> getMedian() > clockAccuracy) {
         printFlag = true;

         if (tResult -> checkStdDev(rangePct, acceptablePct,verboseLvl, false)) {
            /* values are within acceptable deviations */
            done = true;
            break;
         } else {
            if (++count == threshold) {
               done = true;
               break;
            }
         }
      }

      if (printFlag) {
         VerbosePrint::rtmbPrintRetryMsg(clParams, 1, "lock_consistency");
      }

      pthread_mutex_destroy(&mutex);
   }

   listOfResults->push_back(tResult);
   tResult -> setTestResultName(
         "Lock Consistency \
		 test for high priority thread ");

   list<TestScore *> *tScore = generateScores(listOfResults,
         testInfo.getReferenceScore());
   FinalBenchMarkTestResult *fbmt = new FinalBenchMarkTestResult(tScore);

   resultTable.addResult(testInfo, fbmt);

   fflush(stderr);
   fflush(stdout);

   return rc;
}
