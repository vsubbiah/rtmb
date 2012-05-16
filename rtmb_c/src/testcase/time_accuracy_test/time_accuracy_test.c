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
 
#include <time_accuracy_test.h>
#include <errno.h>
/*
 * Name    : time_accuracy()
 * Desc    : Verifies that the timer accuracy wrt to the clock.
 * Args    : args - Test arguments.
 * Returns : NULL
 */

void *time_accuracy(void *args) {
	/* Get the args passed from parent thread */
	thread_args_t *targs = (thread_args_t *) args;
	cmd_line_args *cmd_args = targs->cmd_args;
	benchmark_test *bmtest = targs->bmtest;
	result_table_t *res_table = targs->res_table;
	test_results_t *result = NULL;
	char test_pass;

	/* Other Local variables of this function */
	int rc = SUCCESS;
	int loop = 0;
	struct timespec start_time, end_time;
	long long time_diff;
	int new_iters = bmtest->_iterations;
	long long sleep_time;
	int done = FALSE;
	long long delta;
	struct timespec req_time;

	result = create_test_result(1, new_iters);

	if (result == NULL) {
		RTMB_printf(stderr,
		        "ERROR: Cannot allocate memory for test_results_t");
		RTMB_printf(stderr, " in timer_accuracy()\n");
		abort();
	}

	strcpy((char *) result->desc, (char *) "Time Accuracy test");
	RTMB_verbose_printf(stdout, cmd_args, 1, "\nTest report for %s:\n",
	        (char*) &result->desc);
	RTMB_verbose_printf(stdout, cmd_args, 1,
	        "====================================\n");

	RTMB_verbose_printf(stdout, cmd_args, 1,
	        "\ntime_accuracy: Total number of iterations = %d\n\n",
	        new_iters);

	sleep_time = get_ideal_sleep_time(res_table);

	req_time.tv_sec = sleep_time / 1000000000LL;
	req_time.tv_nsec = sleep_time % 1000000000LL;

	RTMB_verbose_printf(stdout, cmd_args, 3,
	        "NOTE: Considering a sleep time of %lld:%lld  as detected in"
		        " config tests\n", req_time.tv_sec, req_time.tv_nsec);

	for (loop = 0; loop < new_iters; loop++) {
		if (get_cur_time(&start_time) == ERROR) {
			free_chain(result, 0);
			targs->ret = ERROR;
			return (void *) NULL;
		}
		if (nanosleep(&req_time, NULL) != 0) {
			RTMB_printf(stderr, "error code = %d\n", errno);
			abort();
		}
		if (get_cur_time(&end_time) == ERROR) {
			free_chain(result, 0);
			targs->ret = ERROR;
			return (void *) NULL;
		}
		time_diff = get_time_diff(start_time, end_time);

		delta = time_diff - sleep_time;

		RTMB_verbose_printf(stdout, cmd_args, 2,
		        "time_accuracy: Difference between end and "
			        "start times = %.3f us"
			        " \n", MICROSEC(delta));
		if (delta < 0) {
			RTMB_printf(stderr,
			        "time_accuracy: Elapsed time less"
				        " than sleep time in "
				        "iteration %d\n", new_iters);
			test_pass = UNACCEPTABLE_DEVIATIONS;
			abort();
		} else {
			add_entry(result, delta, 0);
		}
	}

	set_test_result_name(result, "time accuracy test ");

	if (check_pass_criteria(result, cmd_args, bmtest, 0) != ERROR) {
		test_pass = ACCEPTABLE_DEVIATIONS;
		rc = SUCCESS;
		done = TRUE;
	} else {
		rc = ERROR;
	}

	add_result_to_result_table2(res_table, result, TIMER_ACCURACY_TEST,
	        bmtest);

	fflush(stderr);
	fflush(stdout);
	targs->ret = SUCCESS;
	return (void *) NULL;
}
