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
 
#include <TestScore.h>

TestScore::TestScore() {
   consistencyPercent = 0;
   minThroughput = 0;
   maxThroughput = 0;
   meanThroughput = 0;
   wgtThroughput = 0;
   subTestScore = 0;
   tResult = NULL;
   testScoreName = "default";
}

TestScore::TestScore(list<TestResults *> *result) {
   consistencyPercent = 0;
   minThroughput = 0;
   maxThroughput = 0;
   meanThroughput = 0;
   wgtThroughput = 0;
   subTestScore = 0;
   tResult = result;
   testScoreName = "default";
}

double TestScore::getConsistencyPercent() const {
   return consistencyPercent;
}

void TestScore::setConsistencyPercent(double consistencyPercent) {
   this->consistencyPercent = consistencyPercent;
}

double TestScore::getMinThroughput() const {
   return minThroughput;
}

void TestScore::setMinThroughput(double minThroughput) {
   this->minThroughput = minThroughput;
}

double TestScore::getMaxThroughput() const {
   return maxThroughput;
}

void TestScore::setMaxThroughput(double maxThroughput) {
   this->maxThroughput = maxThroughput;
}

double TestScore::getMeanThroughput() const {
   return meanThroughput;
}

void TestScore::setMeanThroughput(double meanThroughput) {
   this->meanThroughput = meanThroughput;
}

double TestScore::getWgtThroughput() const {
   return wgtThroughput;
}

void TestScore::setWgtThroughput(double wgtThroughput) {
   this->wgtThroughput = wgtThroughput;
}

double TestScore::getSubTestScore() const {
   return subTestScore;
}

void TestScore::scoreForSubTest(double subTestScore) {
   this->subTestScore = subTestScore;
}

list<TestResults *> *TestScore::getTestResults() const {
   return tResult;
}

void TestScore::setTestResults(list<TestResults *> *tResult) {
   this -> tResult = tResult;
}

void TestScore::setTestResults(TestResults *tResult) {
   list<TestResults *> * listResult = new list<TestResults *> ();
   listResult -> push_back(tResult);
   setTestResults(listResult);
}

void TestScore::setTestScoreName(string testScoreName) {
   this -> testScoreName = testScoreName;
}
string TestScore::getTestScoreName() const {
   return testScoreName;
}
