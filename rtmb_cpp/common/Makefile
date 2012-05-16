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


#Master Makefile to make cpp_impl completely

all:
	cd $(RTMB_SRC_DIR)/harness && make
	cd $(RTMB_SRC_DIR)/testcase && make

.PHONY : clean
clean :
	cd $(RTMB_SRC_DIR)/harness && make clean
	cd $(RTMB_SRC_DIR)/testcase && make clean
