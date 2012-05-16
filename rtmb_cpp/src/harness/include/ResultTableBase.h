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
 
#ifndef __RESULT_TABLE_BASE__
#define __RESULT_TABLE_BASE__

using namespace std;

#include <list>
#include <BenchMarkTestInfo.h>
#include <FinalBenchMarkTestResult.h>

class ResultTableBase {
public:
   virtual void addResult(BenchMarkTestInfo &, list<TestResults*> *)=0;
   virtual void addResult(BenchMarkTestInfo& testInfo, list<TestScore*> *score)=0;
   virtual void addResult(BenchMarkTestInfo& testInfo, FinalBenchMarkTestResult *)=0;
   virtual void addResult(BenchMarkTestInfo &, list<TestResults*> *, bool testPass) =0;
   virtual void addResult(BenchMarkTestInfo& testInfo, list<TestScore*> *score, bool testPass) = 0;
   virtual list<TestResults*> *getTestResultsByTestID(testIds)=0;
   virtual list<TestResults*> *getTestResultsByTestInfo(BenchMarkTestInfo)=0;
   virtual FinalBenchMarkTestResult *getFinalTestResultsByTestInfo(BenchMarkTestInfo testInfo_)=0;
   virtual void setThroughputScoreForSystem(double)=0;
   virtual double getThroughputScoreForSystem()=0;
   virtual void setConsistencyScoreForSystem(double)=0;
   virtual double getConsistencyScoreForSystem()=0;
   virtual void setScoreForSystem(double val)=0;
   virtual double getScoreForSystem()=0;
   virtual void computeScoreForSystem(int)=0;
   virtual map<BenchMarkTestInfo, FinalBenchMarkTestResult *> *getResults()=0;
};
#endif /*__RESULT_TABLE_BASE__*/
