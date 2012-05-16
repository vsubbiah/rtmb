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
 
#ifndef __TEST_RESULTSUTILS__
#define __TEST_RESULTSUTILS__
#include <TestResults.h>
#include <TestScore.h>
#include <ResultTable.h>

long long getClockAccuracy(ResultTable& resultTable);
long long getTimeAccuracy(ResultTable& resultTable);
long long getClockWorkQuantum(ResultTable& resultTable);

typedef struct {
	int iterations;
	int subIterations;
	float rangePct;
	float acceptablePct;
	int verboseLevel;
	bool isVerbose;
	int threshold;
	CmdLineParams* cmdParams;
} ConfigParams;

void initializeConfigParams(ConfigParams& cfgParams, CmdLineParams& cmdParams,
        RealTimeValues& rtValues, int iterations, int subIterations);
double getContribution(long value, long mean);
list<TestScore *> *generateScores(list<TestResults *> *listResult,
        double referenceScore, int singleValued = true);
TestScore *generateCombinedScore(TestResults** tResult, int resultLength,
        double referenceScore);

void printResult(TestResults* result, const char *testName, const char *msg,
        int cmdLineVerboseLvl);
void printResult(TestResults* result, const char *testName, int verboseLvl);
long long getMedianOfClockAccuracy(ResultTable& resultTable);
long long getTimerWorkQuantum(ResultTable& resultTable);
int getNumberOfOnlineCpus();
#endif
