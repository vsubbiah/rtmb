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
 
#ifndef __BENCHMARK_TEST_INFO__
#define __BENCHMARK_TEST_INFO__

#include <string>
#include <BenchMarkTestInfoBase.h>

using namespace std;

class BenchMarkTestInfo: public BenchMarkTestInfoBase {

public:

   /** Test name */
   string _testName;

   /** Test description */
   string _testDesc;

   /** Number of iterations of each test */
   int _numIterations;

   /** Is this a configuration test */
   bool _isConfigTest;

   /** Number of times to retry for tighter deviation */
   int _threshold;

   /** reference score for the test */
   double _referenceScore;

   /** constructor */
   BenchMarkTestInfo();

   /** Sets the name of the test */
   void setTestName(string);

   /** Sets the test description */
   void setTestDescription(string);

   /** Sets the number of iterations that the test needs to be run */
   void setIterations(int);

   /**
    * Sets the number of times the retry has to be made during tighter
    * deviation
    */
   void setThreshold(int);

   /** Returns the name of the test */
   string getTestName(void);

   /** Returns the test description */
   string getTestDescription(void);

   bool operator<(BenchMarkTestInfo) const;

   /** Returns the number of iterations */
   int getIterations(void);

   /** Returns whether the test is configuration test or not */
   bool isConfigTest();

   /** Sets the test as config test */
   void setTestAsConfigTest(bool);

   /**
    * Returns the threshold - number of times the test has to retry during
    * tighter deviation
    */
   int getThreshold();

   /** Sets reference score */
   void setReferenceScore(double);

   /** gets reference score */
   double getReferenceScore();
};

#endif /* __BENCHMARK_TEST_INFO__ */
