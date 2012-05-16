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

all: libtest_util.so
libtest_util.so:
	$(CC) $(CFLAGS) -I$(HARNESS_DIR)/include -I$(RTMB_TESTCASE_DIR)/util/math_operations/include -I$(RTMB_TESTCASE_DIR)/util/result_mgmt/include -I$(RTMB_TESTCASE_DIR)/util/thread_oprn/include -I$(RTMB_TESTCASE_DIR)/util/time_mgmt/include -I$(RTMB_TESTCASE_DIR)/util/io_opern/include math_operations/simple_math_opern.c result_mgmt/result_mgmt.c time_mgmt/time_mgmt.c thread_oprn/thread_oprn.c io_opern/disk_io_opern.c -shared -fpic -o libtest_util.so $(LDFLAGS)
	@$(MV) libtest_util.so $(RTMB_LIB_DIR)
