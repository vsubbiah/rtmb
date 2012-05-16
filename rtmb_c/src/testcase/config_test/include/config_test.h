/*******************************************************************************
 * Copyright (c) 2008 International Business Machines Corp.
 * All rights reserved. This program and the accompanying materials
 * are made available under the terms of the Eclipse Public License v1.0
 * which accompanies this distribution, and is available at
 * http://www.eclipse.org/legal/epl-v10.html
 *
 * Contributors:
 *    Ajith Ramanath            - initial API and implementation
 *    Radhakrishnan Thangamuthu - initial API and implementation
 *    Mike Fulton               - initial API and implementation
 *******************************************************************************/
 
#ifndef __CONFIG_TEST__
#define __CONFIG_TEST__

/* All system includes go in here */
#include <pthread.h>
#include <time.h>
#include <string.h>

/* For print related utilities */
#include <RTMB_print.h>

/* From framework code */
#include <RTMB_rd_cmd_line.h>
#include <RTMB_parse_in_file.h>
#include <RTMB_launch_tests.h>

/* From util code of tests */
#include <time_mgmt.h>
#include <result_mgmt.h>
#include <thread_oprn.h>

/* All the defines go in here */
#define DEF_SZ_CLK_CONF 100
#define NANOSLEEP_TIME	2000
#define DEF_DISKIO_SZ 512
#define DEF_NETIO_SZ 512
#define SERVER_PORT 1703
#define SUCCESS 0
#define ERROR -1
#define DEFAULT_SUBITERS 2

#define MS 1000000LL
#define HIGHEST_PRIO_VAL sched_get_priority_max(SCHED_FIFO)
#define HIGH_PRIO_VAL (HIGHEST_PRIO_VAL-10)
#define MAX_HOST_NAME_LEN  200

#define ACCEPTABLE_DEVIATIONS 1
#define UNACCEPTABLE_DEVIATIONS 0

#define ACCEPTABLE_RANGE_CFG_TEST 20
#define ACCEPTABLE_PCT_CFG_TEST 99.9

/* All structs and related data go below */
typedef struct server_arguments {
	int stay_alive;
	long int port_number;
	int iterations;
} server_arguments_t;

/* All function declarations for config test below */
void *clock_config(void *);
void *timer_config(void *);
void *mcache_perf_config(void *);
void *int_rate_config(void *);
void *float_rate_config(void *);
void *disk_io_config(void *);
void *net_io_config(void *);
void cleanup(server_arguments_t *, pthread_t, char *, int);

#endif /*__CONFIG_TEST__*/
