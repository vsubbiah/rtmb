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
 
#include <sys/param.h>
#include <iostream>
#include <unistd.h>
#include <stdlib.h>

#include <CmdLineParams.h>
#include <HomeDirBuilder.h>

using namespace std;

CmdLineParams::CmdLineParams(int argc, char **cmdLine) {
   setDefaults();
   parse(argc, cmdLine);
}

CmdLineParams::~CmdLineParams() {
   delete &_fileName;
   delete &_testNames;
   delete &_dirName;
}

void CmdLineParams::setDefaultInputFileName() {
   string *tmp = new string(HomeDirBuilder::getHomeDir());
   tmp->append("/config/input.txt");
   _fileName = *tmp;
}

void CmdLineParams::setRTMBBinDir() {
   string *tmp = new string(HomeDirBuilder::getHomeDir());
   tmp->append("/bin/");
   _rtmbBinDir = *tmp;
}

void CmdLineParams::allocateTestNames() {
   list<string> *strs = new list<string> ();
   _testNames = *strs;
}

void CmdLineParams::setDefaults() {
   setDefaultInputFileName();
   setDefaultReportDir();
   setRTMBBinDir();
   allocateTestNames();
   /* Set the rest */
   _isVerbose = false;
   _quickRun = false;
   _rtMode = SOFT_RT;
   _verboseLevel = 0;
}

void CmdLineParams::setDefaultReportDir() {
   char reportDir[MAXPATHLEN + 1];
   if (getcwd(reportDir, MAXPATHLEN + 1) == NULL) {
      cerr << "Error while getting the current working directory" << endl;
      abort();
   }
   string *dir = new string(reportDir);
   _dirName = *dir;
}

void CmdLineParams::parse(int numOfArgs_, char **cmd_) {
   int i;
   bool initialized(true);

   for (i = 1; i < numOfArgs_; i++) {
      string cmdLine(cmd_[i]);
      if (cmdLine == "--help" || cmdLine == "-h" || cmdLine == "-?") {
         usage(0);
      } else if (cmdLine == "--verbose:simple" || cmdLine == "-v:simple") {
         _isVerbose = true;
         _verboseLevel = 1;
      } else if (cmdLine == "--verbose:detailed" || cmdLine == "-v:detailed") {
         _isVerbose = true;
         _verboseLevel = 2;
      } else if (cmdLine == "--silent" || cmdLine == "-s") {
         _isVerbose = false;
      } else if (cmdLine == "--file" || cmdLine == "-f") {
         if (cmd_[i + 1] != NULL) {
            _fileName = cmd_[i + 1];
         } else {
            cerr << "Invalid option specified "
               "on the command line" << endl;
            initialized = false;
         }
         i++;
      } else if (cmdLine == "--report" || cmdLine == "-r") {
         if (cmd_[i + 1] != NULL) {
            _dirName = cmd_[i + 1];
         } else {
            cerr << "Invalid option specified "
               "on the command line" << endl;
            initialized = false;
         }
         i++;
      } else if (cmdLine == "--quick" || cmdLine == "-q") {
         _quickRun = true;
      } else if (cmdLine == "--compliant" || cmdLine == "-c") {
         _quickRun = false;
      } else if (cmdLine == "--test" || cmdLine == "-t") {
         if (cmd_[i + 1] != NULL) {
            string tmpStr(cmd_[i + 1]);
            setTestNames(tmpStr);
         } else {
            cerr << "Invalid option specified "
               "on the command line" << endl;
            initialized = false;
         }
         i++;
      } else if (cmdLine == "--test?" || cmdLine == "-t?") {
         printTestList();
      } else if (cmdLine == "--hard-rt") {
         _rtMode = HARD_RT;
      } else if (cmdLine == "--soft-rt") {
         _rtMode = SOFT_RT;
      } else if (cmdLine == "--guaranteed-rt") {
         _rtMode = GUARANTEED_RT;
      } else {
         initialized = false;
      }
   }
   if (!initialized) {
      usage(-1);
   } else {
      if (_isVerbose) {
         printSelectedOptions();
      }
   }
}

void CmdLineParams::usage(int exitCode) {
   cout << "Usage: rtmb [<option>]*, where <option> is one of:\n" << endl;
   cout << "--help or -? or -h\t\tPrint the command-line syntax." << endl;
   cout << "--silent or s\t\t\tSilent run (default)." << endl;
   cout << "--verbose or -v\t\t\tVerbose output. Opposite of -s." << endl;
   cout << "--file <file> or -f <file>\tPath for input file. " << endl
         << "\t\t\t\tEx:\"-f ./config/input.txt\"" << endl;
   cout << "--report <dir> or -r <dir>\tPath to report directory. " << endl
         << "\t\t\t\tDefaults to $PWD." << endl;
   cout << "--quick or -q\t\t\tPerform a quick single iteration run." << endl;
   cout << "--compliant or -c\t\tPerform compliant run. Opposite of -q "
         << "(default)." << endl;
   cout << "--test [<test>] or -t [<test>]\tTests to run separated by "
      "colon." << " Default = all." << endl;
   cout << "--hard-rt\t\t\tRun in hard real-time mode." << endl;
   cout << "--soft-rt\t\t\tRun in soft real-time mode." << endl;
   cout << "-t, --test [<tests>]*\t\tTests to run separated by colon. "
         << "Default = all." << endl;
   cout << "\t\t\t\tType '-t?' or '--test?' to print list of tests.\n" << endl;
   exit(exitCode);
}

void CmdLineParams::printTestList() {
   cout << "Usage: rtmb [-t|--test] [<tests>]*" << endl;
   cout << "Where <tests> could be one or more of:" << endl;
   cout << "\ntime_accuracy\t\tVerifies that the clock and timer work "
         << "consistently." << endl;
   cout << "event_dispatch\t\tVerifies events can be dispatched with "
         << "consistent overhead." << endl;
   cout << "thread_priority\t\tVerifies higher priority threads preempt "
         << "lower priority threads." << endl;
   cout << "lock_consistency\tVerifies that acquiring locks without "
         << "contention runs consistently." << endl;
   cout << "periodic_events\t\tVerifies that periodic real-time events"
         << " are dispatched consistently." << endl;
   cout << "multi-cpu\t\tVerifies that systems with multiple CPUs "
      "can scale reasonably across multiple runtime environments "
      "without loss of determinism." << endl;
   cout << "multi_thread\t\tVerifies that systems with multiple cpus can"
         << " scale reasonably across\n \t\t\tmultiple application "
            "threads" << "in a single process." << endl << endl;
   exit(0);
}

void CmdLineParams::printSelectedOptions() {
   cout << "========================================================="
      "=======================" << endl;
   cout << "Command Line options:" << endl;
   cout << endl;
   cout << "Input Parameter file: " << _fileName << "." << endl;
   cout << "Report Directory: " << _dirName << "." << endl;
   if (_isVerbose) {
      cout << "Verbose mode enabled." << endl;
   } else {
      cout << "Verbose mode disabled." << endl;
   }
   if (_quickRun) {
      cout << "Performing a quick single iteration run." << endl;
   } else {
      cout << "Performing a full compliant run." << endl;
   }
   list<string> testList = _testNames;
   if (!testList.empty()) {
      cout << "Tests requested to be run:" << endl;
      list<string>::iterator testNameIter;
      for (testNameIter = testList.begin(); testNameIter != testList.end(); testNameIter++) {
         cout << *testNameIter << "\t";
      }
      cout << endl;
   } else {
      cout << "No user specified tests. Running all the tests." << endl;
   }
   cout << "========================================================="
      "=======================" << endl;
   cout << endl;
}

string CmdLineParams::getInputFilename() {
   return _fileName;
}

void CmdLineParams::setTestNames(string test_) {
   int newPos;
   string parsedText(test_);
   while ((newPos = parsedText.find(":", 0)) != string::npos) {
      _testNames.push_back(parsedText.substr(0, newPos));
      parsedText = parsedText.substr(newPos + 1);
   }
   // In case there is a single test without ':', push again
   _testNames.push_back(parsedText);
}

list<string> CmdLineParams::getTestNames() {
   return _testNames;
}

bool CmdLineParams::isVerbose() {
   return _isVerbose;
}

int CmdLineParams::getVerboseLevel() {
   return _verboseLevel;
}

bool CmdLineParams::isHardRealTime() {
   return (_rtMode == HARD_RT);
}

bool CmdLineParams::isSoftRealTime() {
   return (_rtMode == SOFT_RT);
}

bool CmdLineParams::isGuaranteedRealTime() {
   return (_rtMode == GUARANTEED_RT);
}

bool CmdLineParams::isQuickRun() {
   return _quickRun;
}

string& CmdLineParams::getReportDir() {
   return _dirName;
}

string& CmdLineParams::getRTMBBinDir() {
   return _rtmbBinDir;
}

