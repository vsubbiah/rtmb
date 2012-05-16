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
 
#include <fstream>
#include <iostream>
#include <assert.h>
#include <limits>
#include <errno.h>
#include <string>
#include <stdlib.h>

using namespace std;

#define MAX_INPUT_LINE_LEN 1024

#include <InputParams.h>

float InputParams::getFloatValues(string line_, string delimiter_) {
   int pos = line_.find(delimiter_, 0);
   float val = 0;

   assert( pos != string::npos && "Format errors in the Input file.");

   string tmpStr = line_.substr(pos + 1);
   char *val_token = (char*) (tmpStr.c_str());

   errno = 0;
   val = atof(val_token);

   assert ( val != 0 && errno == 0);
   return val;
}

int InputParams::getIntValues(string line_, string delimiter_) {
   int pos = line_.find(delimiter_, 0);
   int val = 0;

   assert( pos != string::npos && "Format errors in the Input file.");
   string subString = line_.substr(pos + 1);

   char *val_token = (char*) subString.c_str();

   errno = 0;
   val = atoi(val_token);

   assert ( val != 0 && errno == 0);
   return val;
}

InputParams::InputParams(string inputFileName_) {
   _inputFileName = inputFileName_;
   _tests = *(new list<BenchMarkTestInfo> ());
   parse();
}

InputParams::~InputParams() {
   delete &_tests;
}

void InputParams::parse() {
   char line[MAX_INPUT_LINE_LEN];
   bool newTest(false);
   float val = 0;
   int intVal = 0;
   // A new BenchMarkTest
   BenchMarkTestInfo *bMarkTest = new BenchMarkTestInfo();
   BenchMarkTestInfo emptyTestInfo = *bMarkTest;

   // create ifstream for input file
   ifstream fin(_inputFileName.c_str(), ios::in);
   if (!fin) {
      cerr << "Could not open file " << _inputFileName << endl;
      exit(-1);
   }

   _softRealTimeValues = new RealTimeValues();
   _hardRealTimeValues = new RealTimeValues();

   while (fin.getline(line, 1024)) {
      string theLine(line);
      if (theLine.at(0) == '#') {
         // comment - ignore
         continue;
      } else if (theLine.at(0) == '\n') {
         // newline - ignore
         continue;
      } else if (theLine.at(0) == ' ') {
         // whitespace - ignore
         continue;
      } else if (theLine == "test") {
         if (newTest) {
            cerr << "Bad format in the input file" << _inputFileName << endl;
         }
         newTest = true;

         /* clear the old state in *bMarkTest */
         *bMarkTest = emptyTestInfo;
      } else if (theLine == "/test") {
         if (!newTest) {
            cerr << "Bad format in the input file" << _inputFileName << endl;
         }
         newTest = false;
         _tests.push_back(*bMarkTest);
      } else if (theLine.find("name", 0) != string::npos) {
         // string of form name=timerTest
         assert( newTest );
         bMarkTest->setTestName(getRHSofDelimiter(theLine, "=").c_str());
      } else if (theLine.find("desc", 0) != string::npos) {
         // string of form , desc=Timer accuracy test
         assert( newTest );
         bMarkTest->setTestDescription(getRHSofDelimiter(theLine, "="));
      } else if (theLine.find("threshold", 0) != string::npos) {
         //string of form, threshold=100
         assert( newTest );
         bMarkTest->setThreshold(atoi(getRHSofDelimiter(theLine, "=").c_str()));
      } else if (theLine.find("iterations", 0) != string::npos) {
         // string of form , iterations=100
         assert( newTest );
         bMarkTest->setIterations(atoi(getRHSofDelimiter(theLine, "=").c_str()));
      } else if (theLine.find("reference_score", 0) != string::npos) {
         string tmpStr = getRHSofDelimiter(theLine, "=");
         char *nptr = (char *) tmpStr.c_str();
         double f = strtod(nptr, NULL);
         assert(!(f==0 && errno == ERANGE));
         bMarkTest->setReferenceScore(f);
      } else if (theLine.find("config_test", 0) != string::npos) {
         int isConfig = atoi(getRHSofDelimiter(theLine, "=").c_str());

         if (isConfig == 1) {
            bMarkTest->setTestAsConfigTest(true);
         } else {
            bMarkTest->setTestAsConfigTest(false);
         }

         assert( newTest );
      } else if (theLine.find("soft.range_percentage", 0) != string::npos) {

         val = getFloatValues(theLine, "=");
         _softRealTimeValues->setRangePercentage(val);
         assert(val>0 && val < 100);
         assert(!newTest );
      } else if (theLine.find("soft.acceptable_percentage", 0) != string::npos) {
         val = getFloatValues(theLine, "=");
         _softRealTimeValues->setAcceptablePercentage(val);
         assert(val>0 && val < 100);
         assert(!newTest );
      } else if (theLine.find("soft.multiplier", 0) != string::npos) {
         intVal = getIntValues(theLine, "=");
         assert(intVal> 0);
         _softRealTimeValues->setMultiplier(intVal);
         assert(!newTest );
      } else if (theLine.find("hard.range_percentage", 0) != string::npos) {

         val = getFloatValues(theLine, "=");
         _hardRealTimeValues->setRangePercentage(val);
         assert(val>0 && val < 100);
         assert(!newTest );
      } else if (theLine.find("hard.acceptable_percentage", 0) != string::npos) {

         val = getFloatValues(theLine, "=");
         _hardRealTimeValues->setAcceptablePercentage(val);
         assert(val>0 && val < 100);
         assert(!newTest );
      } else if (theLine.find("hard.multiplier", 0) != string::npos) {
         intVal = getIntValues(theLine, "=");
         assert(intVal> 0);
         _hardRealTimeValues->setMultiplier(intVal);
         assert(!newTest );
      } else if (theLine.find("guaranteed.range_percentage", 0) != string::npos) {

         val = getFloatValues(theLine, "=");
         _hardRealTimeValues->setRangePercentage(val);
         assert(val>0 && val < 100);
         assert(!newTest );
      } else if (theLine.find("guaranteed.acceptable_percentage", 0)
            != string::npos) {

         val = getFloatValues(theLine, "=");
         _hardRealTimeValues->setAcceptablePercentage(val);
         assert(val>0 && val < 100);
         assert(!newTest );
      } else if (theLine.find("guaranteed.multiplier", 0) != string::npos) {
         intVal = getIntValues(theLine, "=");
         assert(intVal> 0);
         _hardRealTimeValues->setMultiplier(intVal);
         assert(!newTest );
      } else {
         cerr << "WARNING: Invalid parameters "
            "and values in input file --> "
               << getLHSofDelimiter(theLine, "=").c_str() << endl;
      }
   } // close while

   if (newTest) {
      /* This means a "test" entry found in the
       input file without "/test"
       */

      cerr << "WARNING: Invalid test entry --> "
         "a test entry is missing its /test" << endl;
   }
   fin.close(); // close the stream

   delete bMarkTest;
}

/**
 * Name   : InputFileParams::getRHSofDelimiter
 * Desc   : Private method - Tokenizes and gets the right hand side of the
 * 		 delimiter.
 * args   :
 *          line_      = line read from the input file
 *          delimiter_ = Delimiter. '=' is passed down for the moment during
 *           parsing.
 * returns: string - Right hand side sub string of line.
 */

string InputParams::getRHSofDelimiter(string line_, string delimiter_) {
   int pos = line_.find(delimiter_, 0);
   assert( pos != string::npos && "Format errors in the Input file.");
   return line_.substr(pos + 1);
}

/**
 * Name   : InputFileParams::getLHSofDelimiter
 * Desc   : Private method - Tokenizes and gets then left hand side of the
 * 		 delimiter.
 * args   :
 *          line_      = line read from the input file
 *          delimiter_ = Delimiter. '=' is passed down for the moment during
 *          parsing.
 * returns: string - Left hand side sub string of line.
 */

string InputParams::getLHSofDelimiter(string line_, string delimiter_) {
   int pos = line_.find(delimiter_, 0);
   assert( pos != string::npos && "Format errors in the Input file.");
   return line_.substr(0, pos);
}

/**
 *
 */

list<BenchMarkTestInfo> InputParams::getTests() {
   return _tests;
}

/**
 *
 */
int InputParams::getTestListSize() {
   return _tests.size();
}

/**
 *
 */

RealTimeValues &InputParams::getHardRealTimeValues() {
   return *_hardRealTimeValues;
}

/**
 *
 */

RealTimeValues &InputParams::getSoftRealTimeValues() {
   return *_softRealTimeValues;
}

RealTimeValues &InputParams::getGuaranteedRealTimeValues() {
   return *_grtRealTimeValues;
}
