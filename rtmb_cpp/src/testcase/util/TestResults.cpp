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

#include <algorithm>
#include <string>
#include <iostream>
#include <TestResults.h>
#include <VerbosePrint.h>
#include <iomanip>
using namespace std;

pthread_mutex_t _mutex;

TestResults::TestResults(testIds id_, int iterations_) {
   _testId = id_;
   _min = 0;
   _max = 0;
   _mean = 0;
   _stdDev = 0;
   _minUpdated = false;
   _entries = 0;
   _maxEntries = iterations_;
   _iterations = iterations_;
   _values = new long long[iterations_];
   _workQuantum = 0;
   _resultId = 0;
   pthread_mutex_init(&_mutex, (pthread_mutexattr_t *) NULL);
   testResultName = "default";
}

TestResults::TestResults() {
   _min = 0;
   _max = 0;
   _mean = 0;
   _stdDev = 0;
   _minUpdated = false;
   _entries = 0;
   _iterations = 0;
   _values = 0;
   _workQuantum = 0;
   _resultId = 0;
   pthread_mutex_init(&_mutex, (pthread_mutexattr_t *) NULL);
   testResultName = "default";
}

void TestResults::cloneValues(TestResults *src) {
   long long* valueArray = getValues();
   int i;

   (*this) = (*src);

   long long* sourceValues = src -> getValues();

   /*
    * _values array would have been overwritten.
    * copy it back
    */
   _values = valueArray;

   for (i = 0; i < _entries; i++) {
      _values[i] = sourceValues[i];
   }
}

TestResults::~TestResults() {
   _min = 0;
   _max = 0;
   _mean = 0;
   _stdDev = 0;
   _minUpdated = false;
   _entries = 0;
   _iterations = 0;
   _resultId = 0;
   delete[] _values;
   _workQuantum = 0;
   pthread_mutex_destroy(&_mutex);
}

void TestResults::setNumberOfValues(int num) {
   if (_values) {
      delete[] _values;
   }

   _values = new long long[num];
   _maxEntries = num;
}

int TestResults::getNumberOfValues() {
   return _maxEntries;
}

void TestResults::setNumberOfEntries(int num) {
   _entries = num;
}

void TestResults::setIterations(int num) {
   _iterations = num;
}

void TestResults::setTestIds(testIds id_) {
   _testId = id_;
}

void TestResults::setWorkQuantum(int workQuantum_) {
   _workQuantum = workQuantum_;
}

int TestResults::getWorkQuantum() {
   return _workQuantum;
}

void TestResults::setResultId(int resultId_) {
   _resultId = resultId_;
}

int TestResults::getResultId() {
   return _resultId;
}

void TestResults::setMinMaxValues() {
   setMax();
   setMin();
}

void TestResults::setMin() {
   if (!_minUpdated) {
      _min = _values[_entries];
      _minUpdated = true;
   } else {
      if (_values[_entries] < _min) {
         _min = _values[_entries];
      }
   }
}

void TestResults::setMedian() {
   int num = _entries;
   int i = 0;
   long long *tempArray = new long long[_entries];
   int pos1, pos2;

   for (i = 0; i < num; i++) {
      tempArray[i] = _values[i];
   }

   sort(tempArray, tempArray + num);
   if (num % 2 == 0) {
      /* even numbered */

      pos1 = num / 2;
      pos2 = num / 2 - 1;
      _median = (tempArray[pos1] + tempArray[pos2]) / 2.0;
   } else {
      /* odd numbered */
      pos1 = num / 2;
      _median = tempArray[pos1];
   }
}

void TestResults::setMax() {
   if (_values[_entries] > _max) {
      _max = _values[_entries];
   }
}

void TestResults::setValuesAtNextIndex(long long values_) {
   pthread_mutex_lock(&_mutex);
   _values[_entries] = values_;
   setMinMaxValues();
   _entries++;

   if (_entries == _maxEntries) {
      setStdDev();
      setMedian();
   }

   pthread_mutex_unlock(&_mutex);
}

int TestResults::normalizeResults(long long baseVal, long long normalizedVal) {
   int i;
   int count = 0;

   for (i = 0; i < _entries; i++) {
      if (_values[i] < baseVal) {
         _values[i] = normalizedVal;
         count++;
      }
   }

   if (_min < baseVal)
      _min = baseVal;

   if (_max < baseVal)
      _max = baseVal;

   setStdDev();
   setMedian();

   return count;
}

void TestResults::setStdDev() {
   long long diff, sqDiff, totalSqDiff = 0;
   double meanTotalSqDiff = 0;
   long long sum = 0;

   for (int i = 0; i < _entries; i++) {
      sum += _values[i];
   }

   _mean = sum / _entries;

   for (int i = 0; i < _entries; i++) {
      diff = _values[i] - _mean;
      sqDiff = diff * diff;
      totalSqDiff = totalSqDiff + sqDiff;
   }
   meanTotalSqDiff = totalSqDiff * 1.0 / _entries;
   _stdDev = sqrt(meanTotalSqDiff);
}

long long TestResults::getMax() {
   return (_max);
}

long long TestResults::getMin() {
   return (_min);
}

long long TestResults::getMean() {
   return (_mean);
}

double TestResults::getStdDev() {
   return (_stdDev);
}

testIds TestResults::getTestId() {
   return _testId;
}

long long *TestResults::getValues() {
   return _values;
}

int TestResults::getNumberOfEntries() {
   return _entries;
}

int TestResults::getIterations() {
   return _iterations;
}

void TestResults::removeOldValues() {
   _min = 0;
   _max = 0;
   _mean = 0;
   _median = 0;
   _stdDev = 0;
   _minUpdated = false;
   _entries = 0;
}

bool TestResults::checkStdDev(double rangePct, double acceptablePct, int testVerboseLvl, bool synch) {
   bool retVal(false);

   int minAcceptableIters;
   int verboseLvl = 1;
   int i;

   if (synch)
      pthread_mutex_lock(&_mutex);

   setStdDev();

   /* compute whether the deviations are in acceptable range */
   double positiveLimit = 0, negativeLimit = 0;
   int nonConformingIters = 0;
   int conformingIters = 0;

   assert(rangePct> 0.0 && rangePct < 100.0);
   assert(acceptablePct> 0.0 && acceptablePct < 100.0);

   positiveLimit = _median * (1.0 + rangePct / 100.0);
   negativeLimit = _median * (1.0 - rangePct / 100.0);

   for (i = 0; i < _entries; i++) {
      if (_values[i] > positiveLimit || _values[i] < negativeLimit) {
         nonConformingIters++;
      }
   }

   conformingIters = _entries - nonConformingIters;

   minAcceptableIters = (int) (acceptablePct / 100.0 * _entries);
   if (conformingIters >= minAcceptableIters) {
      retVal = true;
   } else {
      retVal = false;
   }

   this ->conformingIterations = conformingIters;
   this ->nonConformingIterations = nonConformingIters;
   this ->positiveLimit = positiveLimit;
   this ->negativeLimit = negativeLimit;

   if (synch)
      pthread_mutex_unlock(&_mutex);

   if (testVerboseLvl >= 1) {

      cout << fixed << PRECISION_3 << endl
            << "Measured median for this test run: " << MICROSEC(
            _median) << " us" << endl;
      cout << fixed << PRECISION_3
            << "Measured maximum time for this test run: " << MICROSEC(_max)
            << " us" << endl;
      cout << fixed << PRECISION_3
            << "Measured minimum time for this test run: " << MICROSEC(_min)
            << " us" << endl;
      cout << fixed << PRECISION_3 << "Measured mean for this test run: "
            << MICROSEC(
                  _mean) << " us" << endl;
      cout << fixed << PRECISION_3
            << "Measured standard deviation for this test run: "
            << MICROSEC(_stdDev) << " us" << endl;
      cout << fixed << PRECISION_3 << "Expected number of iterations between "
            << MICROSEC(negativeLimit) << " us and " << MICROSEC(
            positiveLimit) << " us: " << minAcceptableIters << endl;
      cout << "Total number of conforming iterations: " << conformingIters
            << endl;
      cout << "Total number of outlying iterations: " << nonConformingIters
            << endl;
   }

   return retVal;
}

string TestResults::getTestResultName() const {
   return testResultName;
}

void TestResults::setTestResultName(const char* testResultName) {
   this->testResultName = testResultName;
}

bool TestResults::getTestPass() const {
   return testPass;
}

void TestResults::setTestPass(bool testPass) {
   this->testPass = testPass;
}

double TestResults::getRangePercent() const {
   return rangePercent;
}

void TestResults::setRangePercent(double rangePercent) {
   this->rangePercent = rangePercent;
}

double TestResults::getAcceptablePercent() const {
   return acceptablePercent;
}

void TestResults::setAcceptablePercent(double acceptablePercent) {
   this->acceptablePercent = acceptablePercent;
}

int TestResults::getNonConformingIterations() const {
   return nonConformingIterations;
}

void TestResults::setNonConformingIterations(int nonConformingIterations) {
   this->nonConformingIterations = nonConformingIterations;
}

int TestResults::getConformingIterations() const {
   return conformingIterations;
}

void TestResults::setConformingIterations(int conformingIterations) {
   this->conformingIterations = conformingIterations;
}

double TestResults::getPositiveLimit() const {
   return positiveLimit;
}

void TestResults::setPositiveLimit(double positiveLimit) {
   this->positiveLimit = positiveLimit;
}

double TestResults::getNegativeLimit() const {
   return negativeLimit;
}

void TestResults::setNegativeLimit(double negativeLimit) {
   this->negativeLimit = negativeLimit;
}

double TestResults::getMedian() {
   return _median;
}
