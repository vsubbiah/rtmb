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
#include <TestResultsUtils.h>
#include <VerbosePrint.h>
#include <RealTimeValues.h>
#include <ResultTable.h>

#include <PthreadWrapper.h>
#include <TimeOperations.h>
#include <MathOperations.h>

#include <timer_accuracy.h>

void *_time_accuracy_impl(void *);

extern "C"bool time_accuracy (CmdLineParams &clParams, BenchMarkTestInfo &testInfo,
	RealTimeValues &rtValues, ResultTable &resultTable) {
	pthread_t tid;
	int newIters = testInfo.getIterations();
	bool rc (true);

	TestResults *timerTestResults = NULL;
	list<TestResults*> *listOfTimerTestResults =
	resultTable.getTestResultsByTestID (SYSTIMER_CONFIG);
	if (listOfTimerTestResults != NULL) {
		timerTestResults = listOfTimerTestResults->front();
	}

	/* If we are running quick mode, then we should just do 1 iteration */
	if (clParams.isQuickRun()) {
		newIters = 1;
	} else {
		newIters = newIters * rtValues.getMultiplier();
	}
	testInfo.setIterations (newIters);

	list<TestResults *> *listOfResults = new list<TestResults *>();
	TestResults *tResult = new TestResults(TIMER_ACCURACY_TEST, newIters);
	tResult->setWorkQuantum(timerTestResults->getWorkQuantum());

	bool verbose = clParams.isVerbose();
	int verboseLvl = clParams.getVerboseLevel();
	int threshold = testInfo.getThreshold();
	int iterations = newIters;
	double rangePct = rtValues.getRangePercentage();
	double acceptablePct = rtValues.getAcceptablePercentage();
	long long clockAccuracy = getClockAccuracy(resultTable);

	if (clParams.isVerbose() && (clParams.getVerboseLevel() >= 1) ) {
		cout << endl << "Test Report for time accuracy test:" << endl;
		cout << " ===============================================" << endl;
		cout << endl << "time_accuracy: Total number of iterations: "
		<< newIters << endl;
	}

	bool printFlag = false;
	bool done(false);
	int count = 0;
	long long num = getTimerWorkQuantum(resultTable);
	struct timespec reqTime,startTime,endTime;
	long long timeDiff;
	long long delta;
	int i;

	reqTime.tv_sec = num / 1000000000LL;
	reqTime.tv_nsec = num % 1000000000LL;

	VerbosePrint::rtmbVerbosePrintlnOut(clParams, 3,
		"time_accuracy: second:nanos  ="
		" %lld:%lld", reqTime.tv_sec, reqTime.tv_nsec);

	for( i = 0; i < newIters; i++ ) {
		TimeOperations::getCurTime(&startTime);

		if(nanosleep(&reqTime,NULL) == -1) {
			abort();
		}

		TimeOperations::getCurTime(&endTime);

		timeDiff=TimeOperations::getTimeDiff(endTime,startTime);

		delta = timeDiff - num;
		if( delta < 0 ) {
			VerbosePrint::rtmbVerbosePrintlnErr(
				"invalid time difference");
			abort();
		}

		VerbosePrint::rtmbVerbosePrintlnOut(clParams, 2,
			"time_accuracy: Difference between actual and requested sleep time  ="
			" %.3f us", MICROSEC(delta));

		/* Add this to results */
		tResult->setValuesAtNextIndex(delta);
	}

	if (tResult -> checkStdDev(rangePct, acceptablePct, verboseLvl, false)) {
		/* values are within acceptable deviations */
		done = true;
	}

	tResult -> setTestResultName( "Time accuracy test");
	listOfResults->push_back(tResult);

	list<TestScore *> *tScore = generateScores(listOfResults,
		testInfo.getReferenceScore());
	FinalBenchMarkTestResult *fbmt = new FinalBenchMarkTestResult(tScore);

	resultTable.addResult(testInfo, fbmt);

	return true;
}
