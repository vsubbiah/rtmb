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
 
#include <BenchMarkTestInfo.h>
#include <string.h>

using namespace std;

bool BenchMarkTestInfo::operator<(BenchMarkTestInfo rhs_) const {
   if (strcmp(_testName.c_str(), rhs_._testName.c_str()) < 0) {
      return false;
   }
   return true;
}

BenchMarkTestInfo::BenchMarkTestInfo() {
   _testName = "";
   _testDesc = "";
   _numIterations = 0;
   _isConfigTest = false;
   _threshold = 0;
   _referenceScore = 1;
}

void BenchMarkTestInfo::setTestName(string testName_) {
   _testName = testName_;
}

void BenchMarkTestInfo::setTestDescription(string testDesc_) {
   _testDesc = testDesc_;
}

void BenchMarkTestInfo::setIterations(int iters_) {
   _numIterations = iters_;
}

void BenchMarkTestInfo::setThreshold(int threshold_) {
   _threshold = threshold_;
}

void BenchMarkTestInfo::setTestAsConfigTest(bool val) {
   _isConfigTest = val;
}

void BenchMarkTestInfo::setReferenceScore(double val) {
   _referenceScore = val;
}

string BenchMarkTestInfo::getTestName() {
   return _testName;
}

string BenchMarkTestInfo::getTestDescription() {
   return _testDesc;
}

int BenchMarkTestInfo::getIterations() {
   return _numIterations;
}

bool BenchMarkTestInfo::isConfigTest() {
   return _isConfigTest;
}

int BenchMarkTestInfo::getThreshold() {
   return _threshold;
}

double BenchMarkTestInfo::getReferenceScore() {
   return _referenceScore;
}

