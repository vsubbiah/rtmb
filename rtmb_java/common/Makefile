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

#Master Makefile to make java_impl completely

LIST=$(shell $(FIND) ../src -name "*.java")

$(RTMB_LIB_DIR)/rtmb.jar:$(LIST)
	$(JAVAC) $(LIST) -d $(RTMB_LIB_DIR)
	cd $(RTMB_LIB_DIR) &&  $(JAR) $(JAR_OPT) $(RTMB_JAR) com
	
all: $(RTMB_LIB_DIR)/rtmb.jar

.PHONY : clean
clean :
	$(RM) $(RTMB_LIB_DIR)/rtmb.jar
	$(RMR) $(RTMB_LIB_DIR)/com
