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
#define HIGH_PRIO_VAL HIGHEST_PRIO_VAL-20
#define LOW_PRIO_VAL sched_get_priority_min(SCHED_FIFO)
#define MED_PRIO_VAL ( (HIGH_PRIO_VAL + LOW_PRIO_VAL) / 2 )
#define DEFAULT_SUB_ITERATION 2

typedef enum thread_set_type {
	LOW_PRIORITY, MED_PRIORITY, HIGH_PRIORITY, MAX_PRIO_TYPES
} thread_set_type_t;


typedef struct thread_syncers {
	pthread_mutex_t mutex;
	pthread_cond_t cond;
} thread_syncers_t;

typedef struct thr_args {
	int type;
	int opern_amount;
	cmd_line_args *cmd_args;
	pthread_attr_t *attr;
	int policy;
	int online_cpus;
	test_results_t *results;
	thread_syncers_t* tsyncers;
	thread_syncers_t* parent_sema;
} thr_args_t;

typedef struct thread_id {
	pthread_t tid;
} thread_id_t;

int get_num_online_cpus();

int create_multi_threads( int priority,
        thr_args_t *args_array, test_results_t *result, int start_index,
        int end_index, thread_id_t* thr_id, thread_syncers_t* tsyncers,
        thread_syncers_t* parent_sema);

int run_multi_thread_test(int iters, int sub_iters,
        test_results_t* result_high_priority, test_results_t* result_consolidator, int online_cpus, int total_thrs);

void *start_multi_thread_test(void *);
void lock_inc_counter(	thread_syncers_t* tsyncers, thread_syncers_t* parent_sema );
void lock_dec_counter(	thread_syncers_t* tsyncers, thread_syncers_t* parent_sema );
void cleanup(thr_args_t *high_prio_args, pthread_attr_t *high_prio_attr
	, thread_syncers_t* tsyncers, thread_syncers_t *parent_sema,
	thread_id_t* thr_id);
#endif /*__THR_PRIO_TEST__*/
