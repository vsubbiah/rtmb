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
 
#ifndef __RESULT_TABLE__
#define __RESULT_TABLE__

using namespace std;

#include <map>
#include <BenchMarkTestInfo.h>
#include <FinalBenchMarkTestResult.h>
#include <ResultTableBase.h>

class ResultTable: public ResultTableBase {
private:
   map<BenchMarkTestInfo, FinalBenchMarkTestResult *> _testScoresMap;
   double throughputScoreForSystem;
   double consistencyScoreForSystem;
   double scoreForSystem;
public:
   ResultTable();
   ~ResultTable();
   void addResult(BenchMarkTestInfo &, list<TestResults*> *);
   void addResult(BenchMarkTestInfo &, list<TestResults*> *, bool testPass);
   void addResult(BenchMarkTestInfo& testInfo, list<TestScore*> *score, bool testPass);
   void addResult(BenchMarkTestInfo& testInfo, list<TestScore*> *score);
   void addResult(BenchMarkTestInfo& testInfo, FinalBenchMarkTestResult *);
   list<TestResults*> *getTestResultsByTestID(testIds);
   list<TestResults*> *getTestResultsByTestInfo(BenchMarkTestInfo);
   FinalBenchMarkTestResult *getFinalTestResultsByTestInfo(BenchMarkTestInfo testInfo_);
   void setThroughputScoreForSystem(double);
   double getThroughputScoreForSystem();
   void setConsistencyScoreForSystem(double);
   double getConsistencyScoreForSystem();
   void setScoreForSystem(double val);
   double getScoreForSystem();
   void computeScoreForSystem(int numberOfTests);
   map<BenchMarkTestInfo, FinalBenchMarkTestResult *> *getResults();
};
#endif /*__RESULT_TABLE__*/
