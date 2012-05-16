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

TIME_ACCURACY_HOME=$(RTMB_SRC_DIR)/testcase/time_accuracy
UTIL_HOME=$(RTMB_SRC_DIR)/testcase/util
TEST_UTIL_HOME=$(RTMB_SRC_DIR)/util

TIME_ACCURACY_INPUTS=$(TIME_ACCURACY_HOME)/timer_accuracy.cpp  \
                     $(UTIL_HOME)/TestResults.cpp \
         	      	 $(UTIL_HOME)/TimeOperations.cpp  \
                     $(UTIL_HOME)/PthreadWrapper.cpp  \
                     $(UTIL_HOME)/TestResultUtils.cpp \
                     $(UTIL_HOME)/TestScore.cpp  \
                     $(TEST_UTIL_HOME)/VerbosePrint.cpp  \
                     $(UTIL_HOME)/FinalBenchMarkTestResult.cpp 
