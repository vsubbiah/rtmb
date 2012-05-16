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
 
#include <ResultTable.h>
#include "TestResultsUtils.h"
#include <math.h>

using namespace std;

ResultTable::ResultTable() {
   _testScoresMap = *(new map<BenchMarkTestInfo, FinalBenchMarkTestResult *> ());
}

ResultTable::~ResultTable() {

}

void ResultTable::addResult(BenchMarkTestInfo& testInfo, list<TestResults *> * listOfResults) {

   addResult(testInfo, listOfResults, true);
}

void ResultTable::addResult(BenchMarkTestInfo& testInfo, list<TestResults *> * listOfResults, bool testPass) {

   list<TestScore *> *tScore = generateScores(listOfResults,
         testInfo.getReferenceScore());

   FinalBenchMarkTestResult *fbmt = new FinalBenchMarkTestResult(tScore);

   _testScoresMap.insert(make_pair(testInfo, fbmt));
}

void ResultTable::addResult(BenchMarkTestInfo& testInfo, list<TestScore *> * lst) {
   FinalBenchMarkTestResult * fbmt = new FinalBenchMarkTestResult(lst, true);
   _testScoresMap.insert(make_pair(testInfo, fbmt));
}

void ResultTable::addResult(BenchMarkTestInfo& testInfo, list<TestScore *> * lst, bool testPass) {
   FinalBenchMarkTestResult * fbmt =
         new FinalBenchMarkTestResult(lst, testPass);
   _testScoresMap.insert(make_pair(testInfo, fbmt));
}

void ResultTable::addResult(BenchMarkTestInfo& testInfo, FinalBenchMarkTestResult *fbmt) {
   _testScoresMap.insert(make_pair(testInfo, fbmt));
}
list<TestResults*> *ResultTable::getTestResultsByTestID(testIds id_) {
   map<BenchMarkTestInfo, FinalBenchMarkTestResult *>::iterator iter;

   for (iter = ResultTable::_testScoresMap.begin(); iter
         != ResultTable::_testScoresMap.end(); iter++) {
      BenchMarkTestInfo tInfo = (BenchMarkTestInfo) (iter -> first);
      FinalBenchMarkTestResult *finalResult =
            (FinalBenchMarkTestResult *) (iter -> second);
      list<TestScore *> *tScore = finalResult -> getScore();
      TestScore *firstScore = (TestScore *) tScore -> front();
      list<TestResults *> *t = NULL;
      t = firstScore -> getTestResults();

      if (!t -> empty()) {
         TestResults *tResult = t -> front();

         if (tResult != NULL && (tResult->getTestId() == id_)) {
            return t;
         }
      }
   }

   return NULL;
}

list<TestResults *> *ResultTable::getTestResultsByTestInfo(BenchMarkTestInfo testInfo_) {
   map<BenchMarkTestInfo, FinalBenchMarkTestResult *>::iterator iter;
   FinalBenchMarkTestResult* finalResult;

   for (iter = ResultTable::_testScoresMap.begin(); iter
         != ResultTable::_testScoresMap.end(); iter++) {
      BenchMarkTestInfo bmTestInfo = (iter -> first);

      if (bmTestInfo.getTestName() == testInfo_.getTestName()) {
         finalResult = (FinalBenchMarkTestResult *) (iter -> second);

         list<TestScore *> *tScore = finalResult -> getScore();
         TestScore *firstScore = (TestScore *) tScore -> front();
         list<TestResults *> *t = NULL;
         t = firstScore -> getTestResults();

         return t;
      }
   }

   return NULL;
}

FinalBenchMarkTestResult *ResultTable::getFinalTestResultsByTestInfo(BenchMarkTestInfo testInfo_) {
   map<BenchMarkTestInfo, FinalBenchMarkTestResult *>::iterator iter;
   FinalBenchMarkTestResult* finalResult;

   for (iter = ResultTable::_testScoresMap.begin(); iter
         != ResultTable::_testScoresMap.end(); iter++) {
      BenchMarkTestInfo bmTestInfo = (iter -> first);

      if (bmTestInfo.getTestName() == testInfo_.getTestName()) {
         finalResult = (FinalBenchMarkTestResult *) (iter -> second);
         return finalResult;
      }
   }

   return NULL;
}

double ResultTable::getConsistencyScoreForSystem() {
   return consistencyScoreForSystem;
}

double ResultTable::getThroughputScoreForSystem() {
   return throughputScoreForSystem;
}

void ResultTable::setConsistencyScoreForSystem(double val) {
   consistencyScoreForSystem = val;
}

void ResultTable::setThroughputScoreForSystem(double val) {
   throughputScoreForSystem = val;
}

void ResultTable::setScoreForSystem(double val) {
   scoreForSystem = val;
}

double ResultTable::getScoreForSystem() {
   return scoreForSystem;
}

void ResultTable::computeScoreForSystem(int numberOfTests) {
   map<BenchMarkTestInfo, FinalBenchMarkTestResult *>::iterator iter, scoreIter;
   FinalBenchMarkTestResult* finalResult;
   BenchMarkTestInfo tInfo;

   double consistencyProduct = 1;
   double testScoreProduct = 1;

   double consistentPercent = 1;
   double testScore = 1;

   for (scoreIter = ResultTable::_testScoresMap.begin(); scoreIter
         != ResultTable::_testScoresMap.end(); scoreIter++) {
      finalResult = (FinalBenchMarkTestResult *) (scoreIter -> second);
      tInfo = (BenchMarkTestInfo) scoreIter -> first;

      consistentPercent = finalResult->getConsistencyScoreForTest();
      testScore = finalResult->getThroughputScoreForTest();

      if (consistentPercent > 0)
         consistencyProduct *= pow(consistentPercent, 1.0 / numberOfTests);

      if (testScore > 0)
         testScoreProduct *= pow(testScore, 1.0 / numberOfTests);
   }

   setConsistencyScoreForSystem(consistencyProduct);
   setThroughputScoreForSystem(testScoreProduct);

   double systemScore = pow(consistencyProduct * testScoreProduct, 0.5);
   setScoreForSystem(systemScore);
}

map<BenchMarkTestInfo, FinalBenchMarkTestResult *> * ResultTable::getResults() {
   return &_testScoresMap;
}
