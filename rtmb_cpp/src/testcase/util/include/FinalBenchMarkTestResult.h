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
 
#ifndef FINAL_BENCHMARK__H
#define FINAL_BENCHMARK__H
#include <TestScore.h>

class FinalBenchMarkTestResult {
private:
	list<TestScore *> *score;
	double throughputScoreForTest;
	double consistencyScoreForTest;
	double scoreForTest;
	bool testPass;
public:
	FinalBenchMarkTestResult(list<TestScore *> *tScore, bool testPass =
	        true);
	FinalBenchMarkTestResult(list<TestResults *> *tResult, bool testPass =
	        true);
	list<TestScore*> *getScore() const;
	void computeFinalScores();
	void setScore(list<TestScore*> *score);
	double getThroughputScoreForTest() const;
	void setThroughputScoreForTest(double throughputScoreForTest);
	double getConsistencyScoreForTest() const;
	double getScoreForTest() const;
	void setScoreForTest(double throughputScoreForTest);
	void setConsistencyScoreForTest(double consistencyScoreForTest);
	bool isPassed();
};
#endif
