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

#Master Makefile to make testcase dir 

DIR1=$(RTMB_SRC_DIR)/testcase/config_tests
DIR2=$(RTMB_SRC_DIR)/testcase/time_accuracy
DIR3=$(RTMB_SRC_DIR)/testcase/event_dispatch
DIR4=$(RTMB_SRC_DIR)/testcase/thread_priority
DIR5=$(RTMB_SRC_DIR)/testcase/lock_consistency
DIR6=$(RTMB_SRC_DIR)/testcase/multi_thread
DIR7=$(RTMB_SRC_DIR)/testcase/periodic_event
DIR8=$(RTMB_SRC_DIR)/testcase/multi_cpu

all:
	cd $(DIR1) && make
	cd $(DIR2) && make
	cd $(DIR3) && make
	cd $(DIR4) && make
	cd $(DIR5) && make
	cd $(DIR6) && make
	cd $(DIR7) && make
	cd $(DIR8) && make

.PHONY : clean
clean :
	cd $(DIR1) && make clean
	cd $(DIR2) && make clean
	cd $(DIR3) && make clean
	cd $(DIR4) && make clean
	cd $(DIR5) && make clean
	cd $(DIR6) && make clean
	cd $(DIR7) && make clean
	cd $(DIR8) && make clean
