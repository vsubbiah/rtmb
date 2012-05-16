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
#include <TestResultsUtils.h>
#include <FinalBenchMarkTestResult.h>
#include <RealTimeValues.h>

#include <PthreadWrapper.h>
#include <TimeOperations.h>
#include <DiskIOReadUtils.h>
#include <VerbosePrint.h>
#include <MathOperations.h>
#include <MatrixOperations.h>
#include <errno.h>
#include <sys/wait.h>
#include <fstream>

#include <multi_cpu_test.h>

/*
 * Name     : multi_cpu()
 * Desc     : Tests if processes of different priorities can run on multiple CPUs.
 * Args     : args - Test arguments passed from harness.
 * Returns  : NULL.
 */
static long long clockAccuracy, timeAccuracy;
static int iterations;
static CmdLineParams *cmdLineParams;

#define MIN_SLEEP_UNITS 2
#define MIN_WORK_UNITS  1
#define FILL_FILNAME(filename,y)  sprintf((filename),"/tmp/multicpu_%d",(y))

static string multicpuPath("");

void statusCheck(bool & retryStatus, bool& testStatus, double median) {
   /*
    *  Mean should be more than 1 units of clock Accuracy and 2 units of time Accuracy
    */
   if (((MIN_SLEEP_UNITS * median) > timeAccuracy) && (MIN_WORK_UNITS * median)
         > clockAccuracy) {
      testStatus = true;
   } else {
      testStatus = false;
   }
}

pid_t startChildProcess(ChildProcs* childInfo, int subIters, int iterations,
      int timeToSleep, unsigned long timeToStart) {
   char iterationString[20];
   char subIterString[20];
   char timeToSleepString[20];
   char timeToStartString[20];
   char priorityString[20];
   char needLogString[20];
   char idString[20];
   pid_t pid;

   sprintf(iterationString, "%d", iterations);
   sprintf(subIterString, "%d", subIters);
   sprintf(timeToSleepString, "%d", timeToSleep);
   sprintf(timeToStartString, "%lu", timeToStart);
   sprintf(priorityString, "%d", childInfo -> priority);
   sprintf(idString, "%s", childInfo -> filename);

   if (childInfo -> priority == HIGH_PRIO_VAL) {
      strcpy(needLogString, "1");
   } else {
      strcpy(needLogString, "0");
   }

   VerbosePrint::rtmbVerbosePrintlnOut(*cmdLineParams, 2,
         " %s %s %s %s %s %s %s %s", "multi_cpu_impl", subIterString,
         iterationString, timeToSleepString, timeToStartString, idString,
         priorityString, needLogString);

   pid = fork();

   if (pid == 0) {
      string arg0 = multicpuPath;
      arg0.append("multi_cpu_impl");

      /* child process */
      execlp(arg0.c_str(), arg0.c_str(), subIterString, iterationString,
            timeToSleepString, timeToStartString, idString, priorityString,
            needLogString, NULL);

      perror(" execlp:");

      /* When execlp fails due to error , abort */
      exit(-1);
   } else if (pid > 0) {
      childInfo -> pid = pid;
   } else {
      /* hard failure. test cannot run */
      abort();
   }

   return pid;
}

bool scanFile(char *filename, TestResults& tResult, int iterations) {
   ifstream in(filename);
   long long timeDiff;
   bool status = true;

   for (int i = 0; i < iterations; i++) {
      in >> timeDiff;
      tResult.setValuesAtNextIndex(timeDiff);
   }

   return status;
}

extern "C" bool multi_cpu(CmdLineParams &clParams, BenchMarkTestInfo &testInfo,
      RealTimeValues &rtValues, ResultTable &resultTable) {

   int newIters;
   newIters = testInfo.getIterations();
   bool rc(true);

   int verboseLvl = clParams.getVerboseLevel();
   bool isVerbose = clParams.isVerbose();

   double rangePct = rtValues.getRangePercentage();
   double acceptablePct = rtValues.getAcceptablePercentage();
   int onlineCpus;

   multicpuPath = clParams.getRTMBBinDir();

   /* If we are running quick mode, then we should just do 1 iteration */
   if (clParams.isQuickRun()) {
      newIters = 1;
   } else {
      newIters = newIters * rtValues.getMultiplier();
   }

   iterations = newIters;
   testInfo.setIterations(newIters);

   cmdLineParams = &clParams;

   int i = 0, j = 0;

   /* Get the total number of online CPUs on the system */
   onlineCpus = getNumberOfOnlineCpus();

   if (onlineCpus == 1) {
      VerbosePrint::rtmbVerbosePrintlnOut(*cmdLineParams, 2,
            "multi_cpu: Test not done on uniprocessor systems ");
      return (void *) false;
   }

   if (clParams.isVerbose() && (clParams.getVerboseLevel() >= 1)) {
      cout << endl << "Test Report for multi_cpu test:" << endl;
      cout << "===============================================" << endl;
      cout << endl << "multi_cpu: Total number of iterations = " << newIters
            << endl;
   }

   TestResults *tResult = new TestResults;
   tResult -> setTestIds(MULTI_CPU_TEST);
   tResult -> setNumberOfValues(newIters);

   clockAccuracy = getClockAccuracy(resultTable);
   timeAccuracy = getTimeAccuracy(resultTable);

   if (clockAccuracy < 0 || timeAccuracy < 0) {
      VerbosePrint::rtmbVerbosePrintlnOut(*cmdLineParams, 2,
            "multi_cpu: clockAccuracyMaxTime = %lld "
               "timeAccuracy = %lld  "
               "unable to get timer or clock accuracy  ");
      return (void *) false;
   }

   int subIters = 2;
   bool done(false);
   int retryNumber = -1;
   bool retryStatus(false), testStatus(false);

   while (!done) {
      tResult -> removeOldValues();
      retryNumber++;

      {
         MatrixMultiplier mult(subIters, newIters);
         long long* val = mult.getDiffTime();
         mult.runIterations();

         for (int i = 0; i < newIters; i++) {
            tResult -> setValuesAtNextIndex(val[i]);
         }

         if (isVerbose && verboseLvl >= 3)
            printResult(tResult, "multithread_sample_run", 2);
      }

      bool rc = tResult->checkStdDev(rtValues.getRangePercentage(),
            rtValues.getAcceptablePercentage(),
            clParams.getVerboseLevel(), false);

      statusCheck(retryStatus, testStatus, tResult->getMedian());

      if (!testStatus) {
         subIters *= 2;
         done = false;
      } else {
         /* test passed */
         done = true;
         VerbosePrint::rtmbVerbosePrintlnOut(*cmdLineParams, 2,
               "multi_cpu: sample run passed  ");
      }
   }

   time_t now = time(NULL);
   time_t timeToStart = now + onlineCpus * 5;

   TestResults *multiCpuResult = new TestResults[onlineCpus + 1];
   TestResults& summaryResult = multiCpuResult[onlineCpus];

   summaryResult.setTestIds(MULTI_CPU_TEST);
   summaryResult.setNumberOfValues(onlineCpus);
   summaryResult.setIterations(newIters);

   done = false;
   retryNumber = 0;

   while (!done) {
      testStatus = runMultiCpuTest((long long) tResult -> getMedian(),
            onlineCpus, newIters, subIters, timeToStart, multiCpuResult,
            rtValues, clParams);

      if (testStatus) {
         done = true;
         break;
      } else {
         retryNumber++;

         if (retryNumber == testInfo.getThreshold()) {
            done = true;
            break;
         }

         for (i = 0; i <= onlineCpus; i++) {
            multiCpuResult[i].removeOldValues();
         }
      }
   }

   list<TestResults *> *listOfResults = new list<TestResults *> ();

   for (i = 0; i < onlineCpus; i++)
      listOfResults -> push_back(&multiCpuResult[i]);

   list<TestScore *> *tScore = generateScores(listOfResults,
         testInfo.getReferenceScore());
   FinalBenchMarkTestResult *fbmt = new FinalBenchMarkTestResult(tScore);
   resultTable.addResult(testInfo, fbmt);

   return true;
}

bool runMultiCpuTest(long long workTime, int onlineCpus, int newIters,
      int subIters, unsigned long timeToStart, TestResults *multiCpuResult,
      RealTimeValues &rtValues, CmdLineParamsBase &cmdLineParams) {

   TestResults& summaryResult = multiCpuResult[onlineCpus];
   ChildProcs* childInfo = new ChildProcs[2 * onlineCpus];
   int i;
   bool rc(false);

   bool testStatus(true);

   for (i = 0; i < 2 * onlineCpus; i++) {
      childInfo[i].pid = 0;
      childInfo[i].exited = false;
      childInfo[i].priority = 0;
   }

   for (i = onlineCpus; i < 2 * onlineCpus; i++) {
      childInfo[i].priority = LOW_PRIO_VAL;
      FILL_FILNAME(childInfo[i].filename,i);
      childInfo[i].pid = startChildProcess(&childInfo[i], (2 * subIters),
            newIters, (2 * workTime), timeToStart);
   }

   for (i = 0; i < onlineCpus; i++) {
      childInfo[i].priority = HIGH_PRIO_VAL;
      FILL_FILNAME(childInfo[i].filename,i);
      childInfo[i].pid = startChildProcess(&childInfo[i], subIters, newIters,
            (4 * workTime), (unsigned long) timeToStart);
   }

   int childsToExit = (2 * onlineCpus);
   int status;

   while (childsToExit) {
      pid_t exitedPid = waitpid(-1, &status, 0);

      if (WIFEXITED(status) && (WEXITSTATUS(status) == 0)) {
         /* normal exit */
      } else {
         VerbosePrint::rtmbVerbosePrintlnErr("multi_cpu: ERROR. child process "
            "exited  abnormally ");
         return false;
      }

      for (i = 0; i < 2 * onlineCpus; i++) {
         if ((!childInfo[i].exited) && (childInfo[i].pid == exitedPid)) {
            childInfo[i].exited = true;
            childsToExit--;
            break;
         }
      }
   }

   for (i = 0; i < onlineCpus; i++) {
      TestResults& result = multiCpuResult[i];
      char buf[30];
      sprintf(buf, " Multi CPU test for CPU #%d", i);
      result.setTestResultName(buf);
      result.setTestIds(MULTI_CPU_TEST);
      result.setNumberOfValues(newIters);
      result.setIterations(newIters);

      if (scanFile(childInfo[i].filename, result, newIters) == false) {
         /* hard failure */
         abort();
      }
      unlink(childInfo[i].filename);

      rc = result.checkStdDev(rtValues.getRangePercentage(),
            rtValues.getAcceptablePercentage(),
            cmdLineParams.getVerboseLevel(), false);

      if (!rc) {
         testStatus = false;
      }

      if (cmdLineParams.isVerbose()) {
         printResult(&result, "multi_cpu", cmdLineParams.getVerboseLevel());
      }

      summaryResult.setValuesAtNextIndex(result.getMean());
   }

   summaryResult.setTestResultName("Multi Cpu test  -- summary result ");

   if (cmdLineParams.isVerbose()) {
      printResult(&summaryResult, "multi_cpu", cmdLineParams.getVerboseLevel());
   }

   rc = summaryResult.checkStdDev(rtValues.getRangePercentage(),
         rtValues.getAcceptablePercentage(),
         cmdLineParams.getVerboseLevel(), false);

   if (!rc) {
      testStatus = false;
   }

   delete[] childInfo;

   return testStatus;
}
