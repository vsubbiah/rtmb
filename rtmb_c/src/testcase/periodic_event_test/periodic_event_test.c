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
 
#include <periodic_event_test.h>
#include <stdio.h>
#include <time.h>
#include <signal.h>
#include <unistd.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <sys/time.h>

static struct timespec start_time;
static struct timespec period;
static int events_to_fire;
static int events_fired;
static struct timespec *dispatch_times = NULL;
volatile long done = 0;
static timer_t timer_id;
static sigset_t sig_set;
static struct itimerspec fire_time;
static struct sigevent evp;

long long timeAccuracy;

pthread_mutex_t event_lock;

int do_run(test_results_t *, int, int);
int check_results(result_table_t *, test_results_t *, cmd_line_args *,
        benchmark_test *, int);
int add_results(result_table_t *, test_results_t *, cmd_line_args *,
        benchmark_test *, int);
void timer_expiry_handler(int);

/*
 * Name     : periodic_event()
 * Desc     : Tests if one thread is notified by other thread.
 * Args     : args - Test arguments passed from harness.
 * Returns  : NULL.
 */
void *periodic_event(void *args) {
	/* Get the args passed from parent thread */
	thread_args_t *targs = (thread_args_t *) args;
	benchmark_test *bmtest = targs->bmtest;
	result_table_t *res_table = targs->res_table;
	cmd_line_args *cmd_args = targs ->cmd_args;
	test_results_t *results = NULL;
	int rc = SUCCESS;
	int run_status = ERROR;
	char test_pass = UNACCEPTABLE_DEVIATIONS;
	int sub_iters;

	/* Locals */
	int new_iters = bmtest->_iterations;

	RTMB_verbose_printf(stdout, cmd_args, 1,
	        "\nTest report for periodic_event test:\n");
	RTMB_verbose_printf(stdout, cmd_args, 1,
	        "====================================\n");
	RTMB_verbose_printf(stdout, cmd_args, 1,
	        "\nperiodic_event: Total number of iterations = %d\n\n",
	        new_iters);


	timeAccuracy = get_ideal_sleep_time(res_table);

	for (sub_iters = 1; sub_iters <= bmtest->_threshold; sub_iters++) {
		if (dispatch_times != NULL) {
			free(dispatch_times);
		}

		if (results != NULL) {
			delete_test_result(results);
		}

		results = create_test_result(1, new_iters);
		if (results == NULL) {
			RTMB_printf(stderr,
			"ERROR: Cannot allocate memory for "
				"test_results_t");
			RTMB_printf(stderr, " in periodic_event\n");
			targs->ret = ERROR;
			return (void *) NULL;
		}
		run_status = do_run(results, bmtest->_iterations, sub_iters);
		if (run_status == SUCCESS) {
			add_results(res_table, results, targs->cmd_args,
			        bmtest, new_iters);
		} else {
			RTMB_verbose_printf(stdout, cmd_args, 1, "\n");
			RTMB_verbose_printf(stdout, cmd_args, 1,
			        "periodic_event_test: Retrying with a larger "
				        "time interval...\n");
			continue;
		}
		rc = check_results(res_table, results, targs->cmd_args, bmtest,
		        new_iters);
		if (rc == SUCCESS) {
			test_pass = ACCEPTABLE_DEVIATIONS;
			break;
		} else {
			RTMB_verbose_printf(stdout, cmd_args, 1, "\n");
			RTMB_verbose_printf(stdout, cmd_args, 1,
			        "periodic_event_test: Retrying with a larger "
				        "time interval..\n");
			continue;
		}
	}

	if (sub_iters == bmtest -> _threshold + 1) {
		RTMB_printf(stderr,
		"periodic_event: exceeded"
			" maximum attempts \n");
	}

	fflush(stderr);
	fflush(stdout);
	if (dispatch_times != NULL) {
		free(dispatch_times);
	}
	targs->ret = SUCCESS;
	add_result_to_result_table2(res_table, results, PERIODIC_EVENTS_TEST,
	        bmtest);
	return (void *) NULL;
}

int add_results(result_table_t* res_table, test_results_t *results,
        cmd_line_args * cmd_args, benchmark_test *bmtest, int iterations) {
	int i;
	long long per = convert_time_to_nsec(period);
	long long expected_fire_time = convert_time_to_nsec(start_time);

	for (i = 0; i < iterations; i++) {
		long long time_diff = convert_time_to_nsec(dispatch_times[i])
		        - expected_fire_time;
		RTMB_verbose_printf(stdout , cmd_args, 2,
		        "periodic_event: Difference between expected start and"
			        " actual start = %.3f us "
			        "\n", MICROSEC(time_diff));

		expected_fire_time = expected_fire_time + per;
		/* Now add the time diff to the table */
		if (add_entry(results, time_diff, 0) == ERROR) {
			RTMB_printf(stderr, " Unable to add time diff in "
				"periodic_event \n");
			return ERROR;
		}
	}

	return SUCCESS;
}

int check_results(result_table_t* res_table, test_results_t *results,
        cmd_line_args * cmd_args, benchmark_test *bmtest, int iterations) {
	int rc = SUCCESS;
	if (check_pass_criteria(results, cmd_args, bmtest, 0) == ERROR) {
		rc = ERROR;
	} else {
		rc = SUCCESS;
	}

	return rc;
}

int do_run(test_results_t * result, int iterations, int sub_iters) {
	int rc;

	struct sigaction sig_act;
	pthread_mutex_init(&event_lock, NULL);

	dispatch_times = (struct timespec *) calloc(iterations,
	        sizeof(struct timespec));
	if (dispatch_times == NULL) {
		return -1;
	}

	/* Set up timer: */
	memset(&evp, 0, sizeof(evp));

	evp.sigev_notify = SIGEV_SIGNAL;
	evp.sigev_signo = SIGUSR1;
	evp.sigev_value.sival_int = 0;

	rc = sigemptyset(&sig_set);
	if (rc != 0) {
		RTMB_printf(stderr, "ERROR: during sigemptyset");
		return ERROR;
	}
	sig_act.sa_handler = (void(*)(int)) timer_expiry_handler;
	sig_act.sa_flags = 0;

	rc = sigemptyset(&sig_act.sa_mask);
	if (rc != 0) {
		RTMB_printf(stderr, "ERROR: during sigemptyset1");
		return ERROR;
	}

	rc = sigaction(evp.sigev_signo, &sig_act, 0);
	if (rc != 0) {
		RTMB_printf(stderr, "ERROR: during sigemptyset1 %d\n", errno);
		return ERROR;
	}

	rc = clock_gettime(CLOCK_MONOTONIC, &start_time);

	if (rc != 0) {
		RTMB_printf(stderr, "ERROR: during timer_gettime %d \n",
		        errno);
		return ERROR;
	}

	rc = timer_create(CLOCK_MONOTONIC, &evp, &timer_id);
	if (rc != 0) {
		RTMB_printf(stderr, "ERROR: during timer_create\n");
		perror("timer create");
		return ERROR;
	}

	start_time.tv_sec += 5;
	start_time.tv_nsec = 0;

	period.tv_sec = ( timeAccuracy * sub_iters ) / 1000000000LL;
	period.tv_nsec = ( timeAccuracy * sub_iters ) % 1000000000LL;;
	events_to_fire = iterations;
	done = 0;
	events_fired = 0;

	fire_time.it_value.tv_sec = start_time.tv_sec;
	fire_time.it_value.tv_nsec = start_time.tv_nsec;
	fire_time.it_interval.tv_sec = period.tv_sec;
	fire_time.it_interval.tv_nsec = period.tv_nsec;
	rc = timer_settime(timer_id, TIMER_ABSTIME, &fire_time, NULL);
	if (rc != 0) {
		RTMB_printf(stderr, "ERROR: during timer_settime\n");
		return ERROR;
	}

	while (!done) {
		struct timespec timeout;
		timeout.tv_sec = 30;
		timeout.tv_nsec = 0;
		sigtimedwait(&sig_set, NULL, &timeout);
	}

	fire_time.it_interval.tv_sec = 0;
	fire_time.it_interval.tv_nsec = 0;
	timer_settime(timer_id, TIMER_ABSTIME, &fire_time, NULL);
	timer_delete(timer_id);

	return SUCCESS;
}

void timer_expiry_handler(int signo) {
	struct timespec fired_time;

	if (clock_gettime(CLOCK_MONOTONIC, &fired_time) < 0) {
		perror("clock_gettime:");
		abort();
	}

	if (signo == SIGUSR1 && !done) {
		if (events_fired < events_to_fire) {
			dispatch_times[events_fired] = fired_time;
			events_fired += 1;
		} else {
			done = 1;
			timer_delete(timer_id);
		}
	}
}

/*
 * sigval_t defined in bits/siginfo.h in Linux
 * sigval_t is not defined in AIX . Used union sigval as sigval_t in AIX
 * -- reference sys/signal.h
 */
