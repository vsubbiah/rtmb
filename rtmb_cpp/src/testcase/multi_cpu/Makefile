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

include multi_cpu_test.mk 

MULTI_CPU_TEST_LIB=$(RTMB_HOME)/lib/libmulti_cpu.so
MULTI_CPU_TEST_EXE=$(RTMB_HOME)/bin/multi_cpu_impl 
MULTI_CPU_TEST_HOME=$(RTMB_SRC_DIR)/testcase/multi_cpu
UTIL_HOME=$(RTMB_SRC_DIR)/testcase/util

LIB_INC_FLAGS= -I../../harness/include -I../../util/include -I./include -I../util/include  -I../config_tests/include/

EXE_INC_FLAGS= -I../../harness/include -I../../util/include -I./include -I../util/include  -I../config_tests/include/

all: $(MULTI_CPU_TEST_LIB) $(MULTI_CPU_TEST_EXE) 

clean: clean_lib clean_exe

$(MULTI_CPU_TEST_LIB):$(MULTI_CPU_TEST_LIB_INPUTS)
	$(CC) $(CCFLAGS) $(LIB_INC_FLAGS) $(MULTI_CPU_TEST_LIB_INPUTS) -o $@ $(LDFLAGS)


clean_lib:
	$(RM) $(MULTI_CPU_TEST_LIB)

clean_exe:
	$(RM) $(MULTI_CPU_TEST_EXE)
    
$(MULTI_CPU_TEST_EXE):multi_cpu_impl.cpp
	$(CC) $(CCFLAGS) $(EXE_INC_FLAGS) $(MULTI_CPU_TEST_HOME)/multi_cpu_impl.cpp $(UTIL_HOME)/TimeOperations.cpp $(UTIL_HOME)/PthreadWrapper.cpp $(UTIL_HOME)/MatrixOperationsWithObjects.cpp -lpthread -o $(RTMB_BIN_DIR)/multi_cpu_impl -lrt
