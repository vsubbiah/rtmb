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
 
#include <string>
#include <dlfcn.h>
#include <iostream>
#include <list>
#include <pthread.h>

#include <CmdLineParams.h>
#include <InputParams.h>
#include <RTMB.h>
#include <HomeDirBuilder.h>
#include <Reporter.h>
#include <VerbosePrint.h>
#include <RealTimeValues.h>
#include <BenchMarkTestInfo.h>
#include <FinalBenchMarkTestResult.h>
#include <errno.h>
#include <PthreadWrapper.h>
#include <sys/mman.h>
#include <stdlib.h>
#include <string.h>

using namespace std;

// directory where RTMB binary resides
string rtmbBaseDir("");
static int numberOfTests = 0;

void *executeTest(void *args) {
   RTMBArgs *specArgs = (RTMBArgs*) args;

   string testName((specArgs -> bmTest) -> getTestName());

   VerbosePrint::rtmbVerbosePrintlnOut(specArgs -> cmdLineParams, 1,
         "NOTE: Running test %s...", testName.c_str());
   if (!specArgs -> testFunc(*specArgs -> cmdLineParams, *specArgs-> bmTest,
         *specArgs-> rtValues, *specArgs->resultTable)) {
      cerr << "ERROR: Test " << testName.c_str() << " failed." << endl;
      *(specArgs-> rc) = false;
   } else {
      *(specArgs-> rc) = true;
   }

   return (void *) NULL;
}

bool applicableTest(BenchMarkTestInfo& bmTest) {
   const char *testName = bmTest.getTestName().c_str();

   int onlineCpus = sysconf(_SC_NPROCESSORS_ONLN);

   if (onlineCpus > 1)
      return true;
   else
      return (strcmp(testName, "multi_thread") == 0) || (strcmp(testName,
            "multi_cpu") == 0) || (strcmp(testName, "thread_priority") == 0);
}

bool runTest(CmdLineParams &cmdLineParams_, BenchMarkTestInfo &bmTest_,
      RealTimeValues &rtValues, ResultTable &resultTable) {
   bool rc(true);

   string testName(bmTest_.getTestName());

   // Name of the shared object that contains the test impl
   string libName(HomeDirBuilder::getHomeDir() + string("/lib/lib") + testName
         + string(".so"));

   // func pointer to method that implements the test
   bool (*testFunc)(CmdLineParams &, BenchMarkTestInfo &, RealTimeValues &,
         ResultTable &);

   // Get the handle to the shared obj and the func pointer.
   void *libHandle = dlopen(libName.c_str(), RTLD_NOW);
   if (libHandle != NULL) {
      dlerror();
      testFunc
            = (bool(*)(CmdLineParams &, BenchMarkTestInfo &, RealTimeValues &,
                  ResultTable &)) dlsym(libHandle, testName.c_str());
      if (testFunc != NULL) {
         pthread_t starter;

         pthread_attr_t attr;
         pthread_attr_init(&attr);

         int status = PthreadWrapper::setPthreadAttrSchedParam(&attr,
               SCHED_FIFO, sched_get_priority_max(SCHED_FIFO) - 10);

         if (status != 0) {
            cerr << "ERROR: setPthreadAttrSchedParam failed " << endl;
            rc = false;
         } else {
            RTMBArgs s = { &cmdLineParams_, &bmTest_, &rtValues, &resultTable,
                  testFunc, &rc };

            if (pthread_create(&starter, &attr, executeTest, &s) == -1) {
               int errCode = errno;
               cerr << "ERROR: pthread_create error_code:"
                  "= " << errCode << endl;
               rc = false;
            } else if (pthread_join(starter, NULL) == -1) {
               int errCode = errno;
               cerr << "ERROR: pthread_join error_code:" << errCode << endl;
               rc = false;
            }
         }
      } else {
         cerr << "ERROR: Method " << testName.c_str() << "is not found in "
               << libName.c_str() << endl;
         rc = false;
      }
      dlclose(libHandle);
   } else {
      // if libHandle == NULL
      cerr << "Error: " << dlerror() << endl;
      cerr << "Implementation of test " << testName.c_str() << " not found.";
      cerr << " Skipping it..." << endl;
      rc = false;
   }
   return rc;
}

void printScores(BenchMarkTestInfo &bmTest, ResultTable& resultTable,
      CmdLineParams& cmdLineParams) {
   FinalBenchMarkTestResult *finalResult =
         resultTable.getFinalTestResultsByTestInfo(bmTest);

   string testName(bmTest.getTestName());
   bool noScore(false);

   if (finalResult != NULL) {
      list<TestScore *> *tScore = finalResult -> getScore();
      list<TestScore *>::iterator iter;

      int i = 0;

      int len = tScore -> size();
      for (iter = tScore ->begin(); iter != tScore ->end(); iter++) {
         i++;
         TestScore *score = (TestScore *) (*iter);
         cout << endl;

         VerbosePrint::rtmbVerbosePrintlnOut(cmdLineParams, 1, "Scores: ");
         VerbosePrint::rtmbVerbosePrintlnOut(cmdLineParams, 1,
               "Maximum throughput  : %.3f ", score -> getMaxThroughput());
         VerbosePrint::rtmbVerbosePrintlnOut(cmdLineParams, 1,
               "Minimum Throughput  : %.3f ", score -> getMinThroughput());
         VerbosePrint::rtmbVerbosePrintlnOut(cmdLineParams, 1,
               "Mean Throughput     : %.3f ", score -> getMeanThroughput());
         VerbosePrint::rtmbVerbosePrintlnOut(cmdLineParams, 1,
               "Weighted Throughput : %.3f ", score -> getWgtThroughput());

         if (len > 1) {
            VerbosePrint::rtmbVerbosePrintlnOut(cmdLineParams, 1,
                  "Determinism Score   : %.3f ",
                  score -> getConsistencyPercent());
         }
      }

      cout << endl;

      VerbosePrint::rtmbVerbosePrintlnOut(cmdLineParams, 1,
            "Overall Throughput Score for test    : %.3f ",
            finalResult -> getThroughputScoreForTest());

      VerbosePrint::rtmbVerbosePrintlnOut(cmdLineParams, 1,
            "Overall Determinism Score for test    : %.3f ",
            finalResult -> getConsistencyScoreForTest());

      VerbosePrint::rtmbVerbosePrintlnOut(cmdLineParams, 1,
            "Final Score for %s micro-benchmark : %.3f ",
            (char *) bmTest.getTestName().c_str(),
            finalResult->getScoreForTest());
      cout << endl;
   }
}

/**
 *
 */
bool launchTests(CmdLineParams &clParams_, InputParams &iflParams_,
      ResultTable &resultTable) {
   // Test found ?
   bool found(false);
   bool rc(true);

   // Test names
   list<string> cmdLineTestNames = clParams_.getTestNames();
   list<BenchMarkTestInfo> inParamTests = iflParams_.getTests();
   RealTimeValues rtValues;

   if (clParams_.isHardRealTime()) {
      rtValues = iflParams_.getHardRealTimeValues();
   } else if (clParams_.isSoftRealTime()) {
      rtValues = iflParams_.getSoftRealTimeValues();
   } else if (clParams_.isGuaranteedRealTime()) {
      rtValues = iflParams_.getGuaranteedRealTimeValues();
   }

   /**
    * If no tests names are provided in the cmd line run all the
    * tests provided in the input file.
    */
   list<string>::iterator testNameIterator;
   list<BenchMarkTestInfo>::iterator bmTestIterator;

   /* Run the config tests before all tests */
   for (bmTestIterator = inParamTests.begin(); bmTestIterator != inParamTests.end(); bmTestIterator++) {
      BenchMarkTestInfo bmTest = (*bmTestIterator);

      if (bmTest.isConfigTest()) {
         if (!runTest(clParams_, bmTest, rtValues, resultTable)) {
            /* Take action. Do not let config tests fail*/
            cerr << "Problems running configuration test " << bmTest.getTestName() << ". Aborting..." << endl;
            abort();
         } else {
            numberOfTests++;
            printScores(bmTest, resultTable, clParams_);
         }
      }
   }

   if (cmdLineTestNames.size() != 0) {
      /*
       * Iterate to check if the test exists. If exists, launch
       * the test.
       */
      for (testNameIterator = cmdLineTestNames.begin(); testNameIterator != cmdLineTestNames.end(); testNameIterator++) {
         // match the tests from the _cmdLineParams.
         found = false;
         for (bmTestIterator = inParamTests.begin(); bmTestIterator
               != inParamTests.end(); bmTestIterator++) {
            BenchMarkTestInfo bmTest = (*bmTestIterator);
            if ((*testNameIterator) == bmTest.getTestName()) {
               found = true;
               if (!bmTest.isConfigTest()) {
                  // Match found. Run this test...
                  if (!runTest(clParams_, bmTest, rtValues, resultTable)) {

                     /*
                      * test pass for certain tests if running on uniprocessor
                      */
                     if (!applicableTest(bmTest)) {
                        return true;
                     }
                     numberOfTests++;
                     cerr << "No results received for test " << bmTest.getTestName();
                     cerr << "Continuing..." << endl;
                     rc = false;
                  } else {
                     numberOfTests++;
                     printScores(bmTest, resultTable, clParams_);
                  }
               }
            }
         } // end of loop around tests listed in input params.
         // If not found, log error...
         if (!found) {
            cerr << "rtmb WARNING: " << *testNameIterator
                  << " test does not contain an entry in "
                  << " the input file. Skipping it...\n" << endl;
         }
      } // end of loop around tests listed in cmd params tests
   } else {
      // Loop around the tests listed in input files and run 'em !
      for (bmTestIterator = inParamTests.begin(); bmTestIterator != inParamTests.end(); bmTestIterator++) {
         BenchMarkTestInfo bmTest = (*bmTestIterator);
         if (!bmTest.isConfigTest()) {
            if (!runTest(clParams_, bmTest, rtValues, resultTable)) {
               if (!applicableTest(bmTest)) {
                  return true;
               }

               numberOfTests++;
               cerr << "No results received for test " << bmTest.getTestName()
                     << endl;
               cerr << "Continuing..." << endl;
               rc = false;
            } else {
               numberOfTests++;
               printScores(bmTest, resultTable, clParams_);
            }
         }
      }
   }

   return rc;
}

/**
 *
 */
int main(int argc, char **argv) {
   // Construct the home directory at first
   string binName(argv[0]);
   int status = 0;

   /* Lock all pages of the process in memory to avoid paging */
   if (mlockall(MCL_CURRENT | MCL_FUTURE) != 0) {
      cerr << "mlockall failed";
      exit(-1);
   }

   HomeDirBuilder::buildHomeDir(binName);

   int pos = binName.rfind("/");

   if (pos != string::npos) {
      rtmbBaseDir = binName.substr(0, pos);
   } else {
      rtmbBaseDir = "";
   }

   // Instance of cmd line parser
   CmdLineParams *cmdLineParams = new CmdLineParams(argc, argv);

   // Instance of input file parser
   InputParams *iflParams = new InputParams(cmdLineParams->getInputFilename());

   ResultTable *resultTable = new ResultTable();
   bool rc = launchTests(*cmdLineParams, *iflParams, *resultTable);

   Reporter rpt(cmdLineParams);

   string rptDir = cmdLineParams -> getReportDir();

   resultTable->computeScoreForSystem(numberOfTests);

   rpt.generateReport(*resultTable, rptDir);
   VerbosePrint::rtmbVerbosePrintlnOut(cmdLineParams, 1, " ");

   VerbosePrint::rtmbVerbosePrintlnOut(cmdLineParams, 1,
         "Final throughput score for the system: %.3f",
         resultTable->getThroughputScoreForSystem());

   VerbosePrint::rtmbVerbosePrintlnOut(cmdLineParams, 1,
         "Final determinism score for the system: %.3f",
         resultTable->getConsistencyScoreForSystem());

   VerbosePrint::rtmbVerbosePrintlnOut(cmdLineParams, 1, "");

   VerbosePrint::rtmbVerbosePrintlnOut(cmdLineParams, 1,
         "Final score for the system: %.3f", resultTable->getScoreForSystem());
   exit(status);
}

string& getrtmbBaseDir() {
   return rtmbBaseDir;
}
