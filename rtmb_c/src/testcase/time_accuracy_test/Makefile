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

all:    libtime_accuracy.so
libtime_accuracy.so:
		$(CC)  $(CFLAGS) -I$(RTMB_TESTCASE_DIR)/time_accuracy_test/include -I$(RTMB_TESTCASE_DIR)/util/result_mgmt/include -I$(RTMB_TESTCASE_DIR)/util/time_mgmt/include -I$(RTMB_TESTCASE_DIR)/util/thread_oprn/include -I$(RTMB_HARNESS_DIR)/include -I$(RTMB_UTILSRC_DIR)/include time_accuracy_test.c ../util/result_mgmt/result_mgmt.c ../util/time_mgmt/time_mgmt.c ../util/thread_oprn/thread_oprn.c $(RTMB_SRC_DIR)/util/RTMB_print.c ../util/result_mgmt/vector.c -o libtime_accuracy.so $(LDFLAGS)
		@$(MV) libtime_accuracy.so $(RTMB_LIB_DIR)
