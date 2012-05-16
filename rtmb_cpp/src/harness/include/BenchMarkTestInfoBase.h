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
 
#ifndef __BENCHMARK_INFO_BASE__
#define __BENCHMARK_INFO_BASE__

#include <stdio.h>
#include <list>
#include <iostream>
#include <string>

using namespace std;

class BenchMarkTestInfoBase {

public:

   /** Returns the name of the test */
   virtual string getTestName(void)=0;

   /** Returns the test description */
   virtual string getTestDescription(void)=0;

   /** Returns the number of iterations */
   virtual int getIterations(void)=0;

   /** Returns whether the test is configuration test or not */
   virtual bool isConfigTest()=0;

   /** Sets iterations member of the class */
   virtual void setIterations(int)=0;

   /**
    * Returns the threshold - number of times the test has to
    * retry during tighter deviation.
    */
   virtual int getThreshold()=0;

   /**
    * Returns reference score
    */
   virtual double getReferenceScore()=0;

   /**
    * set reference score
    */
   virtual void setReferenceScore(double)=0;
};

#endif /*__BENCHMARK_INFO_BASE__*/
