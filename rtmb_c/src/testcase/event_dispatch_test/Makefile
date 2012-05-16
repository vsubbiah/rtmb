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

EVENT_SOURCE=event_dispatch_test.c signal_notifier.c  \
			  ../util/time_mgmt/time_mgmt.c ../util/thread_oprn/thread_oprn.c \
			  ../util/math_operations/simple_math_opern.c  \
			  ../util/result_mgmt/result_mgmt.c  \
			   $(RTMB_SRC_DIR)/util/RTMB_print.c \
			   ../util/result_mgmt/vector.c
			  
INC_FLAGS=$(RTMB_TESTCASE_DIR)/event_dispatch_test/include  \
   -I$(RTMB_TESTCASE_DIR)/util/result_mgmt/include \
   -I$(RTMB_TESTCASE_DIR)/util/time_mgmt/include \
   -I$(RTMB_TESTCASE_DIR)/util/thread_oprn/include \
   -I$(RTMB_TESTCASE_DIR)/util/math_operations/include \
   -I$(RTMB_HARNESS_DIR)/include \
   -I$(RTMB_UTILSRC_DIR)/include 

all: libevent_dispatch.so

libevent_dispatch.so:
	$(CC) $(CFLAGS) -I$(INC_FLAGS) $(EVENT_SOURCE) -o libevent_dispatch.so $(LDFLAGS)
	@$(MV) libevent_dispatch.so $(RTMB_LIB_DIR)