# *******************************************************************************
# Copyright (c) 2008 International Business Machines Corp.
# All rights reserved. This program and the accompanying materials
# are made available under the terms of the Eclipse Public License v1.0
# which accompanies this distribution, and is available at
# http://www.eclipse.org/legal/epl-v10.html
#
# Contributors:
#    Ajith Ramanath            - initial API and implementation
#    Radhakrishnan Thangamuthu - initial API and implementation
#    Mike Fulton               - initial API and implementation
# *******************************************************************************

HARNESS_HOME = $(RTMB_SRC_DIR)/harness
TEST_UTIL_HOME = $(RTMB_SRC_DIR)/util
REPORTER_HOME= $(RTMB_SRC_DIR)/reporter

RTMB_INPUTS = $(HARNESS_HOME)/CmdLineParams.cpp \
	$(HARNESS_HOME)/InputParams.cpp \
	$(HARNESS_HOME)/BenchMarkTestInfo.cpp \
	$(HARNESS_HOME)/RealTimeValues.cpp \
	$(HARNESS_HOME)/HomeDirBuilder.cpp \
	$(HARNESS_HOME)/ResultTable.cpp \
	$(HARNESS_HOME)/RTMB.cpp \
	$(REPORTER_HOME)/Reporter.cpp \
	$(TEST_UTIL_HOME)/VerbosePrint.cpp \
	$(RTMB_SRC_DIR)/testcase/util/PthreadWrapper.cpp  \
	$(RTMB_SRC_DIR)/testcase/util/TestResults.cpp  \
		$(RTMB_SRC_DIR)/testcase/util/TestResultUtils.cpp  \
	$(RTMB_SRC_DIR)/testcase/util/FinalBenchMarkTestResult.cpp  \
	$(RTMB_SRC_DIR)/testcase/util/TestScore.cpp 
	
