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
 
#ifndef __RTMB__
#define __RTMB__

#include <CmdLineParams.h>
#include <InputParams.h>
#include <ResultTable.h>
#include <BenchMarkTestInfo.h>
#include <BenchMarkTestInfoBase.h>

typedef struct threadArgs {
   CmdLineParams *cmdLineParams;
   BenchMarkTestInfo *bmTest;
   RealTimeValues *realTimeValues;
   list<TestResults> *testResults;
   bool rc;
} threadArgs_t;

typedef struct {
   CmdLineParams *cmdLineParams;
   BenchMarkTestInfo *bmTest;
   RealTimeValues *rtValues;
   ResultTable *resultTable;

   // func pointer to method executing the test
   bool (*testFunc)(CmdLineParams &, BenchMarkTestInfo &, RealTimeValues &, ResultTable &);
   bool *rc; // to store the test pass/fail result
} RTMBArgs;

class RTMB {
private:
   /*Class variables*/
   CmdLineParams _cmdLineParams;
   InputParams _inputParams;
   ResultTable _testResults;
};

string& getRTMBBaseDir();

#endif /*__RTMB__*/
