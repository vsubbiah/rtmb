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
 
#include <event_dispatch.h>

static pthread_attr_t attr;
static long long clock_accuracy;
static CmdLineParams *cmdLineParams;

/*
 * Name  : event_dispatch()
 * Desc  : Wrapper function for disk IO read configuraiton test
 * Args  : clParams      - Cmd line parameters.
 *      testInfo      - Holds Information about the test
 *      rtValues      - Real time values to measure the results against.
 *      resultTable   - Table to store the measured results.
 * Returns      : true on pass and false on fail.
 */
extern "C" bool event_dispatch(CmdLineParams &clParams,
      BenchMarkTestInfo &testInfo, RealTimeValues &rtValues,
      ResultTable &resultTable) {
   pthread_t senderThread, receiverThread;
   pthread_attr_t attr;
   int newIters = testInfo.getIterations();
   long long clockAccuracy = 0;

   /* If we are running quick mode, then we should just do 1 iteration */
   if (clParams.isQuickRun()) {
      newIters = 1;
   } else {
      newIters = newIters * rtValues.getMultiplier();
   }
   testInfo.setIterations(newIters);

   list<TestResults *> *listOfResults = new list<TestResults *> ();
   TestResults *tResult = new TestResults(EVENT_DISPATCH_TEST, newIters);

   bool verbose = clParams.isVerbose();
   int verboseLvl = clParams.getVerboseLevel();
   int threshold = testInfo.getThreshold();
   int iterations = newIters;
   double rangePct = rtValues.getRangePercentage();
   double acceptablePct = rtValues.getAcceptablePercentage();

   cmdLineParams = &clParams;

   initializeEventDispatch();

   bool rc(true);
   bool runStatus(true);
   bool done(false);
   int count = 0;

   evtDispatchMajorIterations = newIters;
   evtDispatchSubIterations = 1;

   clockAccuracy = getClockAccuracy(resultTable);

   if (clParams.isVerbose() && (clParams.getVerboseLevel() >= 1)) {
      cout << endl << "Test Report for event_dispatch test:" << endl;
      cout << "====================================" << endl;
      cout << endl << "event_dispatch: Total number of iterations: "
            << newIters << endl;
   }

   bool printFlag = false;

   while (!done) {
      tResult -> removeOldValues();
      evtDispatchSubIterations *= 2;

      doRun(&senderThread, &receiverThread, tResult, evtDispatchSubIterations,
            evtDispatchMajorIterations);

      if (tResult -> getMedian() > clockAccuracy) {
         printFlag = true;

         if (tResult -> checkStdDev(rangePct, acceptablePct, verboseLvl, false)) {
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
         VerbosePrint::rtmbPrintRetryMsg(clParams, 1, "event_dispatch");
      }
   }

   tResult -> setTestResultName("Event Dispatch test with high priority thread");
   listOfResults->push_back(tResult);

   list<TestScore *> *tScore = generateScores(listOfResults,
         testInfo.getReferenceScore());
   FinalBenchMarkTestResult *fbmt = new FinalBenchMarkTestResult(tScore);

   resultTable.addResult(testInfo, fbmt);

   return true;
}

bool doRun(pthread_t *signalSender, pthread_t *signalReceiver,
      TestResults * result, int subIterations, int iterations) {
   int i;

   evtDispatchSubIterations = subIterations;
   evtDispatchMajorIterations = iterations;

   threadSwitchTime = (long long *) calloc(sizeof(long long),
         evtDispatchMajorIterations);

   if (threadSwitchTime == NULL) {
      VerbosePrint::rtmbVerbosePrintlnOut(*cmdLineParams, 2,
            " unable to allocate memory for threadSwitchTime \n");
      return false;
   }

   startThread(signalSender, signalSenderStart, NULL);
   startThread(signalReceiver, signalReceiverStart, NULL);

   pthread_join(*signalSender, NULL);
   pthread_join(*signalReceiver, NULL);

   for (i = 0; i < evtDispatchMajorIterations; i++) {
      long long timeDiff = threadSwitchTime[i];
      VerbosePrint::rtmbVerbosePrintlnOut(*cmdLineParams, 2,
            "event_dispatch: Difference between end and start times ="
               " %.3f us", MICROSEC(timeDiff));
      /* Now add the task switch time to the result */
      result -> setValuesAtNextIndex(timeDiff);
   }

   free(threadSwitchTime);

   return true;
}
