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
 
#ifndef __LK_CONSISTENCY_TEST__
#define __LK_CONSISTENCY_TEST__

#include <pthread.h>
#include <unistd.h>
#include <assert.h>

/* From util code of tests */
#include <time_mgmt.h>
#include <result_mgmt.h>
#include <thread_oprn.h>
#include <math_opern.h>

/* From framework code */
#include <RTMB_rd_cmd_line.h>
#include <RTMB_parse_in_file.h>
#include <RTMB_launch_tests.h>
#include <RTMB_print.h>

#define SUCCESS 0
#define ERROR -1
#define HIGHEST_PRIO_VAL sched_get_priority_max(SCHED_FIFO)
#define HIGH_PRIO_VAL (HIGHEST_PRIO_VAL-10)

typedef struct thr_args {
	int iterations;
	cmd_line_args *cmd_args;
	pthread_attr_t *attr;
	int policy;
	test_results_t *results;
} thr_args_t;

int create_start_threads(pthread_t *th, pthread_attr_t *attr, thr_args_t *args,
        test_results_t *result, int iterations);

void *run_lock_consistency_test(void *);
void cleanup(thr_args_t *, pthread_attr_t *);

void *lock_consistency(void *);

#endif /*__LK_CONSISTENCY_TEST__*/
