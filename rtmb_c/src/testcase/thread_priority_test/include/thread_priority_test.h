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
 
#ifndef __THR_PRIO_TEST__
#define __THR_PRIO_TEST__

#include <pthread.h>
#include <unistd.h>
#include <assert.h>
#include <stdlib.h>

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
#define HIGH_PRIO_VAL HIGHEST_PRIO_VAL-10
#define LOW_PRIO_VAL sched_get_priority_min(SCHED_FIFO)
#define MED_PRIO_VAL ( (HIGH_PRIO_VAL + LOW_PRIO_VAL) / 2 )

typedef enum thread_set_type {
	LOW_PRIORITY, MED_PRIORITY, HIGH_PRIORITY, MAX_PRIO_TYPES
} thread_set_type_t;

typedef struct thr_args {
	int type;
	int opern_amount;
	int iterations;
	cmd_line_args *cmd_args;
	pthread_attr_t *attr;
	int policy;
	int online_cpus;
	test_results_t *results;
	test_results_t *consolidated_results;
} thr_args_t;

typedef struct thread_id {
	pthread_t tid;
} thread_id_t;

typedef struct thread_syncers {
	pthread_mutex_t mutex;
	pthread_cond_t cond;
} thread_syncers_t;

int get_num_online_cpus();
int create_start_threads(pthread_attr_t *attr, int type, thr_args_t *args,
        test_results_t *result, test_results_t* consolidated_result,
        int opern_amount, int online_cpus, int iterations);

void lock_inc_counter();
void lock_dec_counter();
void *_start(void *);
void cleanup(thr_args_t *, thr_args_t *, thr_args_t *, pthread_attr_t *,
        pthread_attr_t *, pthread_attr_t *);
void *thread_priority(void *);

#endif /*__THR_PRIO_TEST__*/
