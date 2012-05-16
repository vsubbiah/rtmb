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

include $(RTMB_SRC_DIR)/harness/Harness_cpp.mk
INCL_DIR = $(RTMB_SRC_DIR)/harness/include -I$(RTMB_SRC_DIR)/reporter/include  \
		-I$(RTMB_SRC_DIR)/util/include -I$(RTMB_SRC_DIR)/testcase/util/include
		
RTMB = $(RTMB_BIN_DIR)/rtmb
$(RTMB) : $(RTMB_INPUTS)
	$(CC) $(CFLAGS) -I$(INCL_DIR) $(RTMB_INPUTS) -o $@ $(LDPFLAGS)

.PHONY : clean
clean :
	$(RM) $(RTMB_BIN_DIR)/rtmb
