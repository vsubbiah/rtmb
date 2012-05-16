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
 
#ifndef EVENT_DISPATCH_H
#define EVENT_DISPATCH_H
#ifndef EVENT_DISPATCH_PUBLIC
#define EVENT_DISPATCH_PUBLIC extern
#endif
#include <pthread.h>
#include <unistd.h>
/* From util code of tests */
#include <time_mgmt.h>
#include <result_mgmt.h>
#include <thread_oprn.h>
/* From framework code */
#include <RTMB_rd_cmd_line.h>
#include <RTMB_parse_in_file.h>
#include <RTMB_launch_tests.h>
#define HIGHEST_PRIO_VAL sched_get_priority_max(SCHED_FIFO)
#define HIGH_PRIO_VAL HIGHEST_PRIO_VAL-10
#define LOW_PRIO_VAL sched_get_priority_min(SCHED_FIFO)
#define MED_PRIO_VAL ( (HIGH_PRIO_VAL + LOW_PRIO_VAL) / 2 )
#define MIN_SUB_ITERATION 2
#define SUCCESS 0
#define ERROR -1
#define TRUE 1
#define FALSE 0
EVENT_DISPATCH_PUBLIC pthread_mutex_t ready_lock;
EVENT_DISPATCH_PUBLIC pthread_cond_t ready_condn;
EVENT_DISPATCH_PUBLIC pthread_mutex_t wait_notify_lock;
EVENT_DISPATCH_PUBLIC pthread_cond_t wait_notify_condn;

EVENT_DISPATCH_PUBLIC long long *sender_start_time;
EVENT_DISPATCH_PUBLIC long long *receiver_end_time;

void initialize_event_dispatch(void);
void cleanup_event_dispatch(void);

int get_cur_time(struct timespec *tm);
void wait_till_receiver_started(void);
void *signal_sender_start(void *arg);
void *signal_receiver_start(void *arg);
void start_thread(pthread_t *thread, void *(*func)(void *), void *arg);

int run_event_dispatch_test(pthread_t *signal_sender,
        pthread_t *signal_receiver, test_results_t* result, int sub_iterations,
        int major_iterations);

EVENT_DISPATCH_PUBLIC long long ideal_sleep_quantum_time;
EVENT_DISPATCH_PUBLIC int evt_dispatch_sub_iterations;
EVENT_DISPATCH_PUBLIC int evt_dispatch_major_iterations;

#endif
