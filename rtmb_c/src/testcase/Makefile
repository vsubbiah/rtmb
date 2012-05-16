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

all: config_tests time_accuracy_tests thread_priority_tests event_dispatch_tests lock_consistency_tests multi_thread_tests periodic_event_tests 
config_tests:
	@cd $(RTMB_TESTCASE_DIR)/config_test && make
time_accuracy_tests:
	cd $(RTMB_TESTCASE_DIR)/time_accuracy_test && make
thread_priority_tests:
	cd $(RTMB_TESTCASE_DIR)/thread_priority_test && make
event_dispatch_tests:
	cd $(RTMB_TESTCASE_DIR)/event_dispatch_test && make
lock_consistency_tests:
	cd $(RTMB_TESTCASE_DIR)/lock_consistency_test && make
multi_thread_tests:
	cd $(RTMB_TESTCASE_DIR)/multi_thread_test && make
periodic_event_tests:
	cd $(RTMB_TESTCASE_DIR)/periodic_event_test && make
clean: 
	$(RM) $(RTMB_LIB_DIR)/*.so
