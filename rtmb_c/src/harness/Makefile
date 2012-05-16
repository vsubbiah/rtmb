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

INC_FLAGS=$(RTMB_HARNESS_DIR)/include \
          -I$(RTMB_UTILSRC_DIR)/include \
          -I$(RTMB_TESTCASE_DIR)/util/result_mgmt/include \
          -I$(RTMB_TESTCASE_DIR)/util/thread_oprn/include \
          -I$(RTMB_REPORTER_DIR)/include 

RTMB_SOURCES= RTMB_rd_cmd_line.c \
                RTMB_parse_in_file.c  \
                RTMB_launch_tests.c \
                RTMB.c \
               $(RTMB_REPORTER_DIR)/reporter.c \
               $(RTMB_TESTCASE_DIR)/util/result_mgmt/result_mgmt.c \
               $(RTMB_TESTCASE_DIR)/util/result_mgmt/vector.c \
               $(RTMB_TESTCASE_DIR)/util/thread_oprn/thread_oprn.c \
               $(RTMB_SRC_DIR)/util/RTMB_print.c \
               
          
RTMB:	
	$(CC) $(CFLAGS) -I$(INC_FLAGS) $(RTMB_SOURCES) -o rtmb $(LDPFLAGS)
	@$(MV) rtmb $(RTMB_BIN_DIR)
clean:
	$(RM) $(RTMB_BIN_DIR)/rtmb
