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
 
#ifndef __CMD_LN_PARAMS__
#define __CMD_LN_PARAMS__

#include <string>
#include <list>
#include <CmdLineParamsBase.h>

#define SOFT_RT 0
#define HARD_RT 1
#define GUARANTEED_RT 2

using namespace std;

class CmdLineParams: public CmdLineParamsBase {

private:
   /** Set this for verbose output */
   bool _isVerbose;

   /** Level of verbosity */
   int _verboseLevel;

   /** Whether hard realtime or soft realtime */
   int _rtMode;

   /** Input filename */
   string _fileName;

   /** Results directory name */
   string _dirName;

   /** Directory where RTMB bin resides */
   string _rtmbBinDir;

   /** Set this for a quick single iteration of tests */
   bool _quickRun;

   /** Tests to be run */
   list<string> _testNames;

   /** Reads and updates all the default values */
   void setDefaults();
   void setDefaultReportDir();
   void setDefaultInputFileName();
   void allocateTestNames();

   /** Parses the command line and populates the values of this class */
   void parse(int, char **);

   /** Prints the usage details */
   void usage(int);

   /** Prints the list of tests available to be run */
   void printTestList();

   /** Prints all the selected options when running verbose */
   void printSelectedOptions();

   /** set the dir  where RTMB bin is stored */
   void setRTMBBinDir();

public:
   /** constructor */
   CmdLineParams(int, char **);

   /** destructor */
   virtual ~CmdLineParams();

   /** Returns the private input file field */
   string getInputFilename();

   /** Sets the names of the tests that need to be invoked */
   void setTestNames(string);

   /** Returns the private _testNames field */
   list<string> getTestNames();

   /** Returns true if _isVerbose flag is set, else returns false */
   bool isVerbose();

   /** Returns the level of verbosity */
   int getVerboseLevel();

   /** Tells whether the tests are for hard realtime  */
   bool isHardRealTime();

   /** Tells whether the tests are for soft realtime  */
   bool isSoftRealTime();

   /** Tells whether the tests are for guaranteed realtime  */
   bool isGuaranteedRealTime();

   /** Returns true if _quickRun option is set */
   bool isQuickRun();

   /** Returns the report dir  */
   string& getReportDir();

   /** Returns the dir  where RTMB bin is stored */
   string& getRTMBBinDir();
};

#endif /*__CMD_LN_PARAMS__*/
