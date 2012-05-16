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

CONFIG_TESTS_HOME=$(RTMB_SRC_DIR)/testcase/config_tests
UTIL_HOME=$(RTMB_SRC_DIR)/testcase/util

CLOCK_CONFIG_INPUTS=$(CONFIG_TESTS_HOME)/clock_config.cpp $(UTIL_HOME)/TimeOperations.cpp  \
                        $(UTIL_HOME)/PthreadWrapper.cpp $(UTIL_HOME)/MathOperations.cpp \
			$(UTIL_HOME)/TestResults.cpp $(UTIL_HOME)/TestScore.cpp $(UTIL_HOME)/FinalBenchMarkTestResult.cpp  \
            $(UTIL_HOME)/TestResultUtils.cpp $(UTIL_HOME)/../../util/VerbosePrint.cpp 

TIMER_CONFIG_INPUTS= $(CONFIG_TESTS_HOME)/timer_config.cpp $(UTIL_HOME)/TimeOperations.cpp \
			$(UTIL_HOME)/PthreadWrapper.cpp $(UTIL_HOME)/MathOperations.cpp  \
			$(UTIL_HOME)/TestResults.cpp $(UTIL_HOME)/TestScore.cpp \
            $(UTIL_HOME)/TestResultUtils.cpp $(UTIL_HOME)/../../util/VerbosePrint.cpp 


INTRATE_CONFIG_INPUTS=$(CONFIG_TESTS_HOME)/int_rate_config.cpp $(UTIL_HOME)/TimeOperations.cpp \
			$(UTIL_HOME)/PthreadWrapper.cpp $(UTIL_HOME)/MathOperations.cpp  \
			$(UTIL_HOME)/TestResults.cpp $(UTIL_HOME)/TestScore.cpp \
            $(UTIL_HOME)/TestResultUtils.cpp $(UTIL_HOME)/../../util/VerbosePrint.cpp 


FLOATRATE_CONFIG_INPUTS=$(CONFIG_TESTS_HOME)/float_rate_config.cpp $(UTIL_HOME)/TimeOperations.cpp \
			$(UTIL_HOME)/PthreadWrapper.cpp $(UTIL_HOME)/MathOperations.cpp  \
			$(UTIL_HOME)/TestResults.cpp $(UTIL_HOME)/TestScore.cpp \
            $(UTIL_HOME)/TestResultUtils.cpp $(UTIL_HOME)/../../util/VerbosePrint.cpp 

MCACHE_PERF_CONFIG_INPUTS=$(CONFIG_TESTS_HOME)/mcache_perf_config.cpp $(UTIL_HOME)/TimeOperations.cpp \
				$(UTIL_HOME)/PthreadWrapper.cpp $(UTIL_HOME)/ArrayOperations.cpp  \
    			$(UTIL_HOME)/TestResults.cpp $(UTIL_HOME)/TestScore.cpp \
                $(UTIL_HOME)/TestResultUtils.cpp $(UTIL_HOME)/../../util/VerbosePrint.cpp 


DISKIO_READ_CONFIG_INPUTS=$(CONFIG_TESTS_HOME)/disk_io_read_config.cpp $(UTIL_HOME)/TimeOperations.cpp \
				$(UTIL_HOME)/PthreadWrapper.cpp \
    			$(UTIL_HOME)/TestResults.cpp $(UTIL_HOME)/TestScore.cpp \
                $(UTIL_HOME)/TestResultUtils.cpp $(UTIL_HOME)/../../util/VerbosePrint.cpp  \
                $(UTIL_HOME)/DiskIOReadUtils.cpp 

DISKIO_WRITE_CONFIG_INPUTS=$(CONFIG_TESTS_HOME)/disk_io_write_config.cpp $(UTIL_HOME)/TimeOperations.cpp \
				$(UTIL_HOME)/PthreadWrapper.cpp $(UTIL_HOME)/DiskIOWriteUtils.cpp  \
    			$(UTIL_HOME)/TestResults.cpp $(UTIL_HOME)/TestScore.cpp \
                $(UTIL_HOME)/TestResultUtils.cpp $(UTIL_HOME)/../../util/VerbosePrint.cpp 

NET_IO_CONFIG_INPUTS=$(CONFIG_TESTS_HOME)/net_io_config.cpp $(UTIL_HOME)/TimeOperations.cpp \
				$(UTIL_HOME)/PthreadWrapper.cpp \
    			$(UTIL_HOME)/TestResults.cpp $(UTIL_HOME)/TestScore.cpp \
                $(UTIL_HOME)/TestResultUtils.cpp $(UTIL_HOME)/../../util/VerbosePrint.cpp 
