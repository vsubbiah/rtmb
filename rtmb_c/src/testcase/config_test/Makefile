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

INC_FLAGS=$(RTMB_TESTCASE_DIR)/config_test/include\
      -I$(RTMB_TESTCASE_DIR)/util/result_mgmt/include \
      -I$(RTMB_TESTCASE_DIR)/util/time_mgmt/include  \
      -I$(RTMB_TESTCASE_DIR)/util/math_operations/include \
      -I$(RTMB_TESTCASE_DIR)/util/thread_oprn/include \
      -I$(RTMB_TESTCASE_DIR)/util/io_opern/include \
      -I$(RTMB_TESTCASE_DIR)/util/array_operations/include \
      -I$(RTMB_HARNESS_DIR)/include \
      -I$(RTMB_UTILSRC_DIR)/include
  
CONFIG_LIBS=libclock_config.so  \
              libtimer_config.so \
              libmcache_perf_config.so \
              libdisk_io_read_config.so \
              libdisk_io_write_config.so \
              libnet_io_config.so \
              libint_rate_config.so \
              libfloat_rate_config.so

COMMON_SOURCE=../util/result_mgmt/result_mgmt.c \
					 ../util/time_mgmt/time_mgmt.c \
					 ../util/result_mgmt/vector.c  \
					 ../util/thread_oprn/thread_oprn.c  \
					 $(RTMB_SRC_DIR)/util/RTMB_print.c \

CLOCK_CFG_SOURCE=clock_config.c  \
                 ../util/math_operations/simple_math_opern.c 
                       
TIMER_CFG_SOURCE=timer_config.c  \
                 ../util/math_operations/simple_math_opern.c 

MCACHE_CFG_SOURCE=mcache_perf_config.c  \
                   ../util/array_operations/array_operns.c

DISK_IO_RD_CFG_SOURCE=disk_io_read_config.c  	 \
                    ../util/io_opern/disk_io_opern.c

DISK_IO_WR_CFG_SOURCE=disk_io_write_config.c  	 \
                    ../util/io_opern/disk_io_opern.c

NET_IO_CFG_SOURCE=net_io_config.c 

INT_RATE_CFG_SOURCE=int_rate_config.c  \
                    ../util/math_operations/simple_math_opern.c

FLOAT_RATE_CONFIG = float_rate_config.c \
    ../util/math_operations/simple_math_opern.c 

all: $(CONFIG_LIBS) 

libclock_config.so:
	$(CC) $(CFLAGS) -I$(INC_FLAGS) $(CLOCK_CFG_SOURCE) $(COMMON_SOURCE) -o $@ $(LDFLAGS)
	@$(MV) libclock_config.so $(RTMB_LIB_DIR)

libtimer_config.so:
	$(CC) $(CFLAGS) -I$(INC_FLAGS) $(TIMER_CFG_SOURCE) $(COMMON_SOURCE) -o $@ $(LDFLAGS)
	@$(MV) libtimer_config.so $(RTMB_LIB_DIR)

libmcache_perf_config.so:
	$(CC) $(CFLAGS) -I$(INC_FLAGS) $(MCACHE_CFG_SOURCE)  $(COMMON_SOURCE) -o $@  $(LDFLAGS)
	@$(MV) libmcache_perf_config.so $(RTMB_LIB_DIR)
                  
libdisk_io_read_config.so:
	$(CC) $(CFLAGS) -I$(INC_FLAGS) $(DISK_IO_RD_CFG_SOURCE) $(COMMON_SOURCE) -o $@  $(LDFLAGS)
	@$(MV) libdisk_io_read_config.so $(RTMB_LIB_DIR)

libdisk_io_write_config.so:
	$(CC) $(CFLAGS) -I$(INC_FLAGS) $(DISK_IO_WR_CFG_SOURCE) $(COMMON_SOURCE) -o $@  $(LDFLAGS)
	@$(MV) libdisk_io_write_config.so $(RTMB_LIB_DIR)

libnet_io_config.so:
	$(CC) $(CFLAGS) -I$(INC_FLAGS) $(NET_IO_CFG_SOURCE) $(COMMON_SOURCE) -o $@ $(LDFLAGS)
	@$(MV) libnet_io_config.so $(RTMB_LIB_DIR)

libint_rate_config.so:
	$(CC) $(CFLAGS) -I$(INC_FLAGS) $(INT_RATE_CFG_SOURCE) $(COMMON_SOURCE) -o $@ $(LDFLAGS)
	@$(MV) libint_rate_config.so $(RTMB_LIB_DIR)

libfloat_rate_config.so:
	$(CC) $(CFLAGS) -I$(INC_FLAGS) $(FLOAT_RATE_CONFIG)  $(COMMON_SOURCE) -o $@ $(LDFLAGS)
	@$(MV) libfloat_rate_config.so $(RTMB_LIB_DIR)
