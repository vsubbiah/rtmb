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

include thread_priority_test.mk 

THREAD_PRIORITY_TEST_LIB=$(RTMB_HOME)/lib/libthread_priority.so
INC_FLAGS= -I../../harness/include -I../../util/include -I./include -I../util/include  -I../config_tests/include/

all: $(THREAD_PRIORITY_TEST_LIB)

$(THREAD_PRIORITY_TEST_LIB):$(THREAD_PRIORITY_TEST_INPUTS)
	$(CC) $(CCFLAGS) $(INC_FLAGS) $(THREAD_PRIORITY_TEST_INPUTS) -o $@ $(LDFLAGS)

clean:
	$(RM) $(THREAD_PRIORITY_TEST_LIB)
