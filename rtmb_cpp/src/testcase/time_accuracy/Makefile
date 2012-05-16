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

include timer_accuracy.mk 

TIME_ACCURACY_LIB=$(RTMB_HOME)/lib/libtime_accuracy.so
INC_FLAGS=-I../../harness/include -I../../util/include -I./include -I../util/include 

all: $(TIME_ACCURACY_LIB)

$(TIME_ACCURACY_LIB):$(TIME_ACCURACY_INPUTS) timer_accuracy.mk Makefile 
	$(CC) $(CCFLAGS) $(INC_FLAGS) $(TIME_ACCURACY_INPUTS) -o $@ $(LDFLAGS)

clean:
	$(RM) $(TIME_ACCURACY_LIB)
