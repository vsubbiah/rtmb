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

include config_tests.mk 

CLOCK_CONFIG_LIB=$(RTMB_HOME)/lib/libclock_config.so
TIMER_CONFIG_LIB=$(RTMB_HOME)/lib/libtimer_config.so
INTRATE_CONFIG_LIB=$(RTMB_HOME)/lib/libint_rate_test.so
FLOATRATE_CONFIG_LIB=$(RTMB_HOME)/lib/libfloat_rate_test.so
MCACHE_PERF_CONFIG_LIB=$(RTMB_HOME)/lib/libmem_cache_test.so
DISKIO_READ_CONFIG_LIB=$(RTMB_HOME)/lib/libdisk_io_read_test.so
DISKIO_WRITE_CONFIG_LIB=$(RTMB_HOME)/lib/libdisk_io_write_test.so
NET_IO_CONFIG_LIB=$(RTMB_HOME)/lib/libnet_io_test.so

INC_FLAGS=-I../../harness/include -I../../util/include -I./include -I../util/include 

all: $(CLOCK_CONFIG_LIB) $(TIMER_CONFIG_LIB) $(INTRATE_CONFIG_LIB) $(FLOATRATE_CONFIG_LIB)\
$(MCACHE_PERF_CONFIG_LIB) $(DISKIO_READ_CONFIG_LIB) $(DISKIO_WRITE_CONFIG_LIB) $(NET_IO_CONFIG_LIB)

$(CLOCK_CONFIG_LIB):$(CLOCK_CONFIG_INPUTS)
	$(CC) $(CCFLAGS) $(INC_FLAGS) $(CLOCK_CONFIG_INPUTS) -o $@ $(LDFLAGS)

$(TIMER_CONFIG_LIB):$(TIMER_CONFIG_INPUTS)
	 $(CC) $(CCFLAGS) $(INC_FLAGS) $(TIMER_CONFIG_INPUTS) -o $@ $(LDFLAGS)

$(INTRATE_CONFIG_LIB):$(INTRATE_CONFIG_INPUTS)
	$(CC) $(CCFLAGS) $(INC_FLAGS) $(INTRATE_CONFIG_INPUTS) -o $@ $(LDFLAGS)

$(FLOATRATE_CONFIG_LIB):$(FLOATRATE_CONFIG_INPUTS)
	$(CC) $(CCFLAGS) $(INC_FLAGS) $(FLOATRATE_CONFIG_INPUTS) -o $@ $(LDFLAGS)

$(MCACHE_PERF_CONFIG_LIB):$(MCACHE_PERF_CONFIG_INPUTS)
	$(CC) $(CCFLAGS) $(INC_FLAGS) $(MCACHE_PERF_CONFIG_INPUTS) -o $@ $(LDFLAGS)

$(DISKIO_READ_CONFIG_LIB):$(DISKIO_READ_CONFIG_INPUTS)
	$(CC) $(CCFLAGS) $(INC_FLAGS) $(DISKIO_READ_CONFIG_INPUTS) -o $@ $(LDFLAGS)

$(DISKIO_WRITE_CONFIG_LIB):$(DISKIO_CONFIG_INPUTS)
	$(CC) $(CCFLAGS) $(INC_FLAGS) $(DISKIO_WRITE_CONFIG_INPUTS) -o $@ $(LDFLAGS)

$(NET_IO_CONFIG_LIB):$(NET_IO_CONFIG_INPUTS)
	$(CC) $(CCFLAGS) $(INC_FLAGS) $(NET_IO_CONFIG_INPUTS) -o $@ $(LDFLAGS)

clean:
	$(RM) $(CLOCK_CONFIG_LIB)
	$(RM) $(TIMER_CONFIG_LIB)
	$(RM) $(INTRATE_CONFIG_LIB)
	$(RM) $(FLOATRATE_CONFIG_LIB)
	$(RM) $(MCACHE_PERF_CONFIG_LIB)
	$(RM) $(DISKIO_READ_CONFIG_LIB)
	$(RM) $(DISKIO_WRITE_CONFIG_LIB)
	$(RM) $(NET_IO_CONFIG_LIB)
