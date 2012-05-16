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
 
#include <math.h>
#include <pthread.h>

#include <string>
#include <iostream>
#include <VerbosePrint.h>
#include <TestResults.h>
#include <TestResultsUtils.h>
#include <algorithm>

using namespace std;

static long int scale = 1000000;

long long getClockAccuracy(ResultTable& resultTable) {
   TestResults *clockConfig = NULL;
   long long clockAccuracy = -1;

   list<TestResults*> *listOfClockTestResults =
         resultTable.getTestResultsByTestID(SYSCLOCK_CONFIG);
   if (listOfClockTestResults != NULL) {
      clockConfig = listOfClockTestResults->front();

      if (clockConfig != NULL) {
         clockAccuracy = (long long) (20 * clockConfig -> getMedian());
      }
   }

   return clockAccuracy;
}

long long getMedianOfClockAccuracy(ResultTable& resultTable) {
   TestResults *clockConfig = NULL;
   long long clockAccuracy = -1;

   list<TestResults*> *listOfClockTestResults =
         resultTable.getTestResultsByTestID(SYSCLOCK_CONFIG);

   assert(listOfClockTestResults != NULL);

   if (listOfClockTestResults != NULL) {
      clockConfig = listOfClockTestResults->front();

      assert(clockConfig != NULL);

      if (clockConfig != NULL) {
         clockAccuracy = (long long) (clockConfig -> getMedian());
         ;
      }
   }

   return clockAccuracy;
}

long long getClockWorkQuantum(ResultTable& resultTable) {
   TestResults *clockConfig = NULL;
   long long clockWorkQuantum = -1;

   list<TestResults*> *listOfClockTestResults =
         resultTable.getTestResultsByTestID(SYSCLOCK_CONFIG);
   if (listOfClockTestResults != NULL) {
      clockConfig = listOfClockTestResults->front();

      if (clockConfig != NULL) {
         clockWorkQuantum = clockConfig -> getWorkQuantum();
      }
   }
   return clockWorkQuantum;
}

long long getTimerWorkQuantum(ResultTable& resultTable) {
   TestResults *timerConfig = NULL;
   long long timerWorkQuantum = -1;

   list<TestResults*> *listOfTimerTestResults =
         resultTable.getTestResultsByTestID(SYSTIMER_CONFIG);
   if (listOfTimerTestResults != NULL) {
      timerConfig = listOfTimerTestResults->front();

      if (timerConfig != NULL) {
         timerWorkQuantum = timerConfig -> getWorkQuantum();
      }
   }

   return timerWorkQuantum;
}

long long getTimeAccuracy(ResultTable& resultTable) {
   TestResults *timeConfig = NULL;
   long long timeAccuracy = -1;

   list<TestResults*> *listOfTimeTestResults =
         resultTable.getTestResultsByTestID(SYSTIMER_CONFIG);
   if (listOfTimeTestResults != NULL) {
      timeConfig = listOfTimeTestResults->front();

      if (timeConfig != NULL) {
         timeAccuracy = (long long) timeConfig -> getMedian();
      }
   }
   return timeAccuracy;
}

void initializeConfigParams(ConfigParams& cfgParams, CmdLineParams& cmdParams,
      RealTimeValues& rtValues, int iterations, int subIterations) {
   cfgParams.verboseLevel = cmdParams.getVerboseLevel();
   cfgParams.isVerbose = cmdParams.isVerbose();
   cfgParams.acceptablePct = rtValues.getAcceptablePercentage();
   cfgParams.rangePct = rtValues.getRangePercentage();
   cfgParams.iterations = iterations;
   cfgParams.subIterations = subIterations;
   cfgParams.cmdParams = &cmdParams;
}

double getContribution(double value, double median) {
   double percent = 0;
   int contribution;
   double diff = value - median;

   if (diff == 0) {
      percent = 100;
   } else if (diff == -median) {
      percent = 10;
   } else if (diff == median) {
      percent = 0;
   } else {
      /* take absolute value */
      if (diff < 0)
         diff = (-diff);

      if (diff > median) {
         percent = 0;
      } else {
         percent = value * 100.0 / median;

         if (percent > 200.0)
            percent = 0.0;
         else if (percent == 100) {
            percent = 100;
         } else if (percent < 100) {
            percent = (int) (((int) (percent + 10)) / 10);
            percent *= 10;
         } else {
            double difference = percent - 100;
            difference = (int) (((int) (difference)) / 10.0);
            difference = (int) (difference * 10.0);
            percent = 100 - difference;
         }
      }
   }

   return percent;
}

list<TestScore *> *generateScores(list<TestResults *> *listResult,
      double referenceScore, int singleValued) {
   int size = listResult -> size();
   TestResults **tResult = new TestResults *[size];
   long mean = -1;
   list<TestScore *> *listScore = new list<TestScore *> ();
   list<TestResults *>::iterator iter;
   int i = 0;
   iter = listResult -> begin();

   while (iter != listResult -> end()) {
      tResult[i++] = (TestResults *) (*iter);
      iter++;
   }

   if (singleValued) {
      TestScore *tScore = generateCombinedScore(tResult, size, referenceScore);
      tScore -> setTestResults(listResult);
      listScore -> push_back(tScore);
   } else {
      for (int i = 0; i < size; i++) {
         TestScore *tScore = generateCombinedScore(&tResult[i], 1,
               referenceScore);
         tScore -> setTestResults(tResult[i]);
         listScore -> push_back(tScore);
      }
   }

   delete[] tResult;
   return listScore;
}

TestScore *generateCombinedScore(TestResults** tResult, int resultLength,
      double referenceScore) {

   double median = 0;
   TestScore *tScore = new TestScore();

   double contributionPercent = 0;
   double combinedThroughput = 0;
   double minThroughput = 0;
   double maxThroughput = 0;
   double sumThroughput = 0;
   double consistencySum = 0;

   double sum = 0;
   int num = resultLength;
   int i, j, k = 0;
   double *tempArray = new double[num];
   int pos1, pos2;

   for (k = 0; k < num; k++) {
      tempArray[k] = tResult[k]->getMedian();
   }

   sort(tempArray, tempArray + num);
   if (num % 2 == 0) {
      /* even numbered */
      pos1 = num / 2;
      pos2 = num / 2 - 1;
      median = (tempArray[pos1] + tempArray[pos2]) / 2.0;
   } else {
      /* odd numbered */
      pos1 = num / 2;
      median = tempArray[pos1];
   }

   delete[] tempArray;

   int valueLength = 1;
   long long * values = tResult[0] -> getValues();
   maxThroughput = scale * 1.0 / values[0];
   minThroughput = scale * 1.0 / values[0];

   for (i = 0; i < resultLength; i++) {
      values = tResult[i] -> getValues();

      valueLength = tResult[i] -> getNumberOfEntries();
      for (j = 0; j < valueLength; j++) {

         /* Throughput calculation is here */
         double throughput = 1.0 * scale / values[j];

         if (throughput < minThroughput) {
            minThroughput = throughput;
         } else if (throughput > maxThroughput) {
            maxThroughput = throughput;
         }

         sumThroughput += throughput;

         contributionPercent = getContribution(values[j], median);
         combinedThroughput += (contributionPercent * scale) / (values[j]);

         /* consistency calculation is here */
         consistencySum += contributionPercent;
      }
   }

   tScore -> setConsistencyPercent(consistencySum
         / (resultLength * valueLength));
   tScore -> setMinThroughput(minThroughput);
   tScore -> setMaxThroughput(maxThroughput);
   tScore -> setMeanThroughput(sumThroughput / (resultLength * valueLength));
   tScore -> setWgtThroughput(combinedThroughput / (resultLength * valueLength
         * 100.0));

   double interimScore = referenceScore / median;
   double scoreForSubTest = 1;

   /*
    scoreForSubTest *= pow(interimScore, 0.5);
    scoreForSubTest *= pow(tScore->getConsistencyPercent(), 0.5);
    tScore -> scoreForSubTest(scoreForSubTest);
    */

   tScore -> scoreForSubTest(interimScore);

   return tScore;
}

void printResult(TestResults* result, const char *testName, const char *msg,
      int cmdLineVerboseLvl) {
   int requestingVerboseLvl = 2;

   int entries = result -> getNumberOfEntries();
   long long *values = result -> getValues();

   if (cmdLineVerboseLvl >= requestingVerboseLvl) {
      VerbosePrint::rtmbVerbosePrintlnOut(requestingVerboseLvl,
            cmdLineVerboseLvl, (result -> getTestResultName()).c_str());

      for (int i = 0; i < entries; i++) {
         VerbosePrint::rtmbVerbosePrintlnOut(requestingVerboseLvl,
               cmdLineVerboseLvl, "%s: %s = %.3f us", testName, msg, MICROSEC(
                     values[i]));
      }
   }
}

void printResult(TestResults* result, const char *testName, int verboseLvl) {
   printResult(result, testName, "Difference between end and start times ",
         verboseLvl);
}

/*
 * Name    : getNumberOfOnlineCpus()
 * Desc    : Returns the number of online CPUs on the system.
 * Args    : void
 * Returns : Number of online cpus.
 */
int getNumberOfOnlineCpus() {
   return sysconf(_SC_NPROCESSORS_ONLN);
}
