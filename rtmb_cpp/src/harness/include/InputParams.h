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
 
#ifndef __INP_PARAMS__
#define __INP_PARAMS__

#include <list>
#include <BenchMarkTestInfo.h>
#include <RealTimeValues.h>

using namespace std;

class InputParams {

private:

   /** Input filename */
   string _inputFileName;

   /** Lists of tests to be run */
   list<BenchMarkTestInfo> _tests;

   /** Hard real time values */
   RealTimeValues *_hardRealTimeValues;

   /** Soft real time values */
   RealTimeValues *_softRealTimeValues;

   /** Guaranteed hrtRealTimeValues */
   RealTimeValues *_grtRealTimeValues;

   /** Parse the input file */
   void parse();

   /** Tokenizes and gets the right hand side of the delimiter. */
   string getRHSofDelimiter(string, string);

   /** Tokenizes and gets the left hand side of the delimiter. */
   string getLHSofDelimiter(string, string);

public:
   /** Constructor */
   InputParams(string);

   /** Destructor */
   ~InputParams();

   /** Returns the list of tests */
   list<BenchMarkTestInfo> getTests();

   /** Returns the size of the list */
   int getTestListSize();

   /** Returns the hard realtime values */
   RealTimeValues &getHardRealTimeValues();

   /** Returns the soft realtime values */
   RealTimeValues &getSoftRealTimeValues();

   /** Returns the guaranteed realtime values */
   RealTimeValues &getGuaranteedRealTimeValues();

   int getIntValues(string line_, string delimiter_);

   float getFloatValues(string line_, string delimiter_);
};
#endif /*__INP_PARAMS__*/
