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

include periodic_event.mk

PERIODIC_EVENT_LIB=$(RTMB_HOME)/lib/libperiodic_event.so
INC_FLAGS= -I../../harness/include -I../../util/include -I./include -I../util/include  -I../config_tests/include/

all: $(PERIODIC_EVENT_LIB)

$(PERIODIC_EVENT_LIB):$(PERIODIC_EVENT_INPUTS)
	$(CC) $(CCFLAGS)  $(INC_FLAGS) $(PERIODIC_EVENT_INPUTS) -o $@ $(LDFLAGS)

clean:
	$(RM) $(PERIODIC_EVENT_LIB)
