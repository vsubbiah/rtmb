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

MULTI_THREAD_TEST_HOME=$(RTMB_SRC_DIR)/testcase/multi_thread
UTIL_HOME=$(RTMB_SRC_DIR)/testcase/util

MULTI_THREAD_TEST_INPUTS=$(MULTI_THREAD_TEST_HOME)/multi_thread_test.cpp  \
                         $(UTIL_HOME)/TestResults.cpp  \
                         $(UTIL_HOME)/TimeOperations.cpp  \
                         $(UTIL_HOME)/PthreadWrapper.cpp    \
                         $(MULTI_THREAD_TEST_HOME)/../../util/VerbosePrint.cpp  \
                         $(UTIL_HOME)/MathOperations.cpp  \
                         $(UTIL_HOME)/MatrixOperations.cpp \
                         $(UTIL_HOME)/TestScore.cpp  \
                         $(UTIL_HOME)/FinalBenchMarkTestResult.cpp \
                         $(UTIL_HOME)/TestResultUtils.cpp 
