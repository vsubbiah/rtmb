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
 
/*
 * This program measures the task/thread switching time.
 *
 * This test prints difference between the time one thread transferred
 * control to the other and the point in which the dispatch event on the
 * secondary thread got control
 */
#define EVENT_DISPATCH_PUBLIC
#include <event_dispatch_test.h>

#define FALSE 0
#define TRUE 1

static volatile int is_receiver_started = FALSE;

void initialize_event_dispatch() {
	pthread_mutex_init(&wait_notify_lock, NULL);
	pthread_mutex_init(&ready_lock, NULL);
	pthread_cond_init(&wait_notify_condn, NULL);
	pthread_cond_init(&ready_condn, NULL);
}

void cleanup_event_dispatch() {
	pthread_mutex_destroy(&wait_notify_lock);
	pthread_mutex_destroy(&ready_lock);
	pthread_cond_destroy(&wait_notify_condn);
	pthread_cond_destroy(&ready_condn);
}

void wait_till_receiver_started() {
	/* wait till the receiver is on conditional wait */
	pthread_mutex_lock(&ready_lock);

	while (!is_receiver_started) {
		pthread_cond_wait(&ready_condn, &ready_lock);
	}

	/*
	 * reset condition here inorder to use condition
	 * before next iteration
	 */
	is_receiver_started = FALSE;
	pthread_mutex_unlock(&ready_lock);
}

void *signal_sender_start(void * arg) {
	int major_iteration, sub_iteration = 0;
	struct timespec start_time;
	struct timespec end_time;

	for (major_iteration = 0; major_iteration
	        < evt_dispatch_major_iterations; major_iteration++) {
		wait_till_receiver_started();
		pthread_mutex_lock(&wait_notify_lock);

		clock_gettime(CLOCK_MONOTONIC, &start_time);

		for (sub_iteration = 0; sub_iteration
		        < evt_dispatch_sub_iterations; sub_iteration++) {
			/*
			 * receiver is waiting for signal .
			 *  let us send signal
			 */
			pthread_cond_signal(&wait_notify_condn);
			pthread_cond_wait(&wait_notify_condn,
				&wait_notify_lock);
		}

		clock_gettime(CLOCK_MONOTONIC, &end_time);

		pthread_mutex_unlock(&wait_notify_lock);

		receiver_end_time[major_iteration] = end_time.tv_sec
		        * 1000000000LL + end_time.tv_nsec;
		sender_start_time[major_iteration] = start_time.tv_sec
		        * 1000000000LL + start_time.tv_nsec;
	}

	return NULL;
}

void set_receiver_started() {
	pthread_mutex_lock(&ready_lock);
	is_receiver_started = TRUE;
	pthread_cond_signal(&ready_condn);
	pthread_mutex_unlock(&ready_lock);
}

void *signal_receiver_start(void * arg) {
	int major_iteration, sub_iteration = 0;

	for (major_iteration = 0; major_iteration
	        < evt_dispatch_major_iterations; major_iteration++) {
		pthread_mutex_lock(&wait_notify_lock);
		set_receiver_started();

		for (sub_iteration = 0; sub_iteration
		        < evt_dispatch_sub_iterations; sub_iteration++) {
			pthread_cond_wait(&wait_notify_condn, &wait_notify_lock);
			pthread_cond_signal(&wait_notify_condn);
		}

		pthread_mutex_unlock(&wait_notify_lock);
	}

	return NULL;
}

void start_thread(pthread_t *thread, void *func(void *), void *arg) {
	pthread_attr_t attr;
	struct sched_param param;
	int policy = SCHED_FIFO;
	int detS = PTHREAD_EXPLICIT_SCHED;

	pthread_attr_init(&attr);
	pthread_attr_setinheritsched(&attr, detS);

	param.sched_priority = sched_get_priority_max(SCHED_FIFO) - 10;
	pthread_attr_setschedparam(&attr, &param);
	pthread_attr_setschedpolicy(&attr, policy);

	pthread_create(thread, &attr, func, arg);
}
