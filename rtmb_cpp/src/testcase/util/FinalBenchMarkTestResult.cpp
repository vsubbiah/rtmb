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
 
#include <FinalBenchMarkTestResult.h>
#include <math.h>

list<TestScore*> *FinalBenchMarkTestResult::getScore() const {
   return score;
}

void FinalBenchMarkTestResult::setScore(list<TestScore*> *score) {
   this->score = score;
}

double FinalBenchMarkTestResult::getThroughputScoreForTest() const {
   return throughputScoreForTest;
}

void FinalBenchMarkTestResult::setThroughputScoreForTest(
      double throughputScoreForTest) {
   this->throughputScoreForTest = throughputScoreForTest;
}

double FinalBenchMarkTestResult::getScoreForTest() const {
   return scoreForTest;
}

void FinalBenchMarkTestResult::setScoreForTest(double throughputScoreForTest) {
   this-> scoreForTest = throughputScoreForTest;
}

double FinalBenchMarkTestResult::getConsistencyScoreForTest() const {
   return consistencyScoreForTest;
}

void FinalBenchMarkTestResult::setConsistencyScoreForTest(
      double consistencyScoreForTest) {
   this->consistencyScoreForTest = consistencyScoreForTest;
}

bool FinalBenchMarkTestResult::isPassed() {
   return testPass;
}

void FinalBenchMarkTestResult::computeFinalScores() {
   list<TestScore *>::iterator iter;

   int n = score->size();
   double productConsistency = 1;
   double productThroughput = 1;
   double subTestScore;
   double consistencyPercent;
   bool zeroFlag(false);

   for (iter = score->begin(); iter != score->end(); iter++) {
      consistencyPercent = ((TestScore *) (*iter)) -> getConsistencyPercent();
      subTestScore = (*iter) -> getSubTestScore();

      if (consistencyPercent > 0) {
         productConsistency *= pow(consistencyPercent, 1.0 / n);
      } else {
         zeroFlag = true;
      }

      if (productThroughput > 0) {
         productThroughput *= pow(subTestScore, 1.0 / n);
      } else {
         zeroFlag = true;
      }
   }

   setConsistencyScoreForTest(productConsistency);

   if (zeroFlag) {
      setThroughputScoreForTest(0.0);
   } else {
      setThroughputScoreForTest(productThroughput);
      setScoreForTest(pow(productThroughput * productConsistency, 0.5));
   }

}

FinalBenchMarkTestResult::FinalBenchMarkTestResult(list<TestScore *> *tscore,
      bool testPass) {
   this->score = tscore;
   this->throughputScoreForTest = -1;
   this->consistencyScoreForTest = -1;

   this -> testPass = testPass;

   if (testPass) {
      computeFinalScores();
   }
}

FinalBenchMarkTestResult::FinalBenchMarkTestResult(
      list<TestResults *> *tResult, bool testPass) {
   list<TestScore *> *scoreList = new list<TestScore *> ();
   TestScore *tscore = new TestScore(tResult);
   scoreList -> push_back(tscore);
   this->score = scoreList;
   this->testPass = testPass;
   this->throughputScoreForTest = -1;
   this->consistencyScoreForTest = -1;
}

