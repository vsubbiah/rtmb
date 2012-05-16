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
 
/*
 * Reporter.h
 */

#ifndef REPORTER_H_
#define REPORTER_H_
#include <CmdLineParams.h>
#include <ResultTable.h>

class Reporter {
private:
   CmdLineParams* params;

public:
   Reporter(CmdLineParams *);
   virtual ~Reporter();
   void printResultInfo(ofstream&, TestResults* tResult);
   void printScoreInfo(ofstream& pstream, BenchMarkTestInfo& tInfo,
         FinalBenchMarkTestResult *fbtResult);
   void generateReport(ResultTable &table, string& reportDir);
};

#endif /* REPORTER_H_ */
