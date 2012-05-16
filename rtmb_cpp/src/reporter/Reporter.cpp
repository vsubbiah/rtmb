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
 * Reporter.cpp
 */

#include "Reporter.h"
#include <iostream>
#include <fstream>

#include <sys/time.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <errno.h>
#include <VerbosePrint.h>
#include <iomanip>
#include <string.h>

using namespace std;

Reporter::Reporter(CmdLineParams* tParams) {
   this -> params = tParams;
}

Reporter::~Reporter() {
   // TODO Auto-generated destructor stub
}

string* getOutputDir(string& reportDir) {
   struct timeval tval;
   time_t t = time(NULL);
   struct tm *tim = localtime(&t);

   gettimeofday(&tval, NULL);

   char buf[1024];
   char format[] = "cpp_report_YYMMDD_HHMMSS";

   const char* dir = reportDir.c_str();

   if (strlen(dir) + sizeof(format) + 1 > sizeof(buf) - 1) {
      // buffer overflow
      return false;
   }

   // Dont worry abt y3k problem for now
   sprintf(buf, "%s/cpp_report_%02d%02d%02d_%02d%02d%02d%03d", dir,
         (tim->tm_year + 1900) % 100, tim -> tm_mon + 1, tim->tm_mday,
         tim->tm_hour, tim->tm_min, tim->tm_sec, (int) tval.tv_usec / 1000);

   int status = mkdir(buf, 0744);

   if (status != 0) {
      int err = errno;
      VerbosePrint::rtmbVerbosePrintlnErr(" Error Code:=%d", err);
      return NULL;
   }

   return new string(buf);
}

void Reporter::generateReport(ResultTable &table, string& reportDir) {
   map<BenchMarkTestInfo, FinalBenchMarkTestResult *> *m = table.getResults();
   map<BenchMarkTestInfo, FinalBenchMarkTestResult *>::iterator iter;

   string* outputDir = getOutputDir(reportDir);

   if (!outputDir) {
      VerbosePrint::rtmbVerbosePrintlnErr("Error : Unable to generate report directory name");
      return;
   }

   ofstream *pStream;
   for (iter = m -> begin(); iter != m -> end(); iter++) {
      BenchMarkTestInfo tInfo = iter -> first;
      FinalBenchMarkTestResult *fbtResult = iter -> second;

      string fullpath = (*outputDir) + "/" + tInfo.getTestName();

      if (mkdir(fullpath.c_str(), 0744) != 0) {
         VerbosePrint::rtmbVerbosePrintlnErr(
               " Unable to create ouput directory:%s",
               (char *) fullpath.c_str());
         perror("mkdir:");
         continue;
      }

      string testFileName = fullpath + +"/report.txt";

      try {
         pStream = new ofstream(testFileName.c_str());

      } catch (...) {
         VerbosePrint::rtmbVerbosePrintlnErr(
               " Unable to create output file :%s", testFileName.c_str());
         continue;
      }

      try {
         printScoreInfo(*pStream, tInfo, fbtResult);
      } catch (...) {
         VerbosePrint::rtmbVerbosePrintlnErr(
               " Unable to write to  output file :%s ", testFileName.c_str());
         continue;
      }

      pStream -> close();
   }

   string fullpath = (*outputDir) + "/system_report.txt";
   try {
      pStream = new ofstream(fullpath.c_str());
   } catch (...) {
      VerbosePrint::rtmbVerbosePrintlnErr(" Unable to create output file :%s",
            fullpath.c_str());
      return;
   }
   (*pStream) << fixed << PRECISION_3 << "Final Throughput Score "
      "for the system: " << endl << table.getThroughputScoreForSystem() << endl;

   (*pStream) << fixed << PRECISION_3 << "Final Determinism Score "
      "for the system: " << endl << table.getConsistencyScoreForSystem()
         << endl;

   (*pStream) << fixed << PRECISION_3 << endl << "Final Score for the system: "
         << table.getScoreForSystem() << endl;
   pStream -> close();
}

void Reporter::printScoreInfo(ofstream& pStream, BenchMarkTestInfo& tInfo,
      FinalBenchMarkTestResult *fbtResult) {

   list<TestScore *> *tScoreLst = fbtResult -> getScore();

   pStream << "Report for test: " << tInfo.getTestName() << endl;
   pStream << "=======================================" << endl << endl;

   list<TestScore *>::iterator scoreIter;

   int scoreLen = tScoreLst -> size();
   scoreIter = tScoreLst -> begin();

   while (scoreIter != tScoreLst -> end()) {
      TestScore *tScore = (*scoreIter);

      if (scoreLen > 1) {
         pStream << endl << " Scores for subtest:"
               << tScore -> getTestScoreName() << endl;

         pStream << "=======================================" << endl << endl;
      }
      pStream << fixed << PRECISION_3 << "Maximum throughput  :"
            << tScore -> getMaxThroughput();
      pStream << endl << fixed << PRECISION_3 << "Minimum throughput  :"
            << tScore -> getMinThroughput();
      pStream << endl << fixed << PRECISION_3 << "Mean throughput     :"
            << tScore -> getMeanThroughput();
      pStream << endl << fixed << PRECISION_3 << "Weighted throughput :"
            << tScore -> getWgtThroughput();
      pStream << endl << fixed << PRECISION_3 << "Determinism score   :"
            << tScore -> getConsistencyPercent();

      pStream << endl << "Score for test " << tInfo.getTestName() << ": "
            << fbtResult ->getThroughputScoreForTest() << endl;

      list<TestResults *> *testResults = tScore -> getTestResults();
      list<TestResults *>::iterator tResultIter;

      int resultLen = testResults -> size();

      for (tResultIter = testResults -> begin(); tResultIter
            != testResults -> end(); tResultIter++) {

         if (resultLen > 1) {
            pStream << endl << "Test results for: "
                  << (*tResultIter) -> getTestResultName() << endl;
            pStream << "===========================" << "===================="
                  << endl;
         }

         TestResults *tResult = (*tResultIter);
         printResultInfo(pStream, tResult);
      }

      scoreIter++;
   }
}

void Reporter::printResultInfo(ofstream & pStream, TestResults* tResult) {
   pStream << "Total number of iterations of test run: "
         << tResult -> getIterations() << endl;
   pStream << fixed << PRECISION_3 << "Measured median for this test run: "
         << MICROSEC(
               tResult -> getMean()) << " us" << endl;
   pStream << fixed << PRECISION_3 << "Measured maximum for this test run: "
         << MICROSEC(
               tResult -> getMax()) << " us" << endl;
   pStream << fixed << PRECISION_3
         << "Measured minimum time for this test run: " << MICROSEC(
         tResult -> getMin()) << " us" << endl;
   pStream << fixed << PRECISION_3 << "Measured mean for this test run: "
         << MICROSEC(tResult -> getMean()) << " us" << endl;
   pStream << fixed << PRECISION_3
         << "Measured standard deviation for this test run: "
         << MICROSEC(tResult -> getStdDev()) << " us" << endl;

   pStream << "Total number of conforming iterations: "
         << tResult->getConformingIterations() << endl;
   pStream << "Total number of outlying iterations: "
         << tResult->getNonConformingIterations() << endl;

   //Measured execution times in the report should show up for -v:simple
   if (params->getVerboseLevel() >= 1) {
      long long *values = tResult -> getValues();
      pStream << "Measured execution times:" << endl << endl;

      for (int k = 0; k < tResult -> getNumberOfEntries(); k++) {
         pStream << "Execution time " << k << ": " << MICROSEC(
               values[k]) << " us" << endl;
      }
   }
}
