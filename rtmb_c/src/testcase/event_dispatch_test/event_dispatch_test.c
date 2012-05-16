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
 
#include "event_dispatch_test.h"

static double clock_accuracy;
static cmd_line_args *cmd_args;

/*
 * Name     : event_dispatch()
 * Desc     : Tests if one thread is notified by other thread.
 * Args     : args - Test arguments passed from harness.
 * Returns  : NULL.
 */
void *event_dispatch(void *args) {
	/* Get the args passed from parent thread */
	thread_args_t *targs = (thread_args_t *) args;
	cmd_args = targs->cmd_args;
	benchmark_test *bmtest = targs->bmtest;
	result_table_t *res_table = targs->res_table;
	test_results_t *result = NULL;
	int rc = SUCCESS;
	int run_status = ERROR;
	pthread_t sender_thread, receiver_thread;
	int done = FALSE;
	int retry_number = -1;
	char retry_status = UNACCEPTABLE_DEVIATIONS;
	char print_flag = FALSE;

	/* Locals */
	int new_iters = bmtest->_iterations;

	result = create_test_result(1, new_iters);

	if (result == NULL) {
		RTMB_printf(stderr,
		        "ERROR: Cannot allocate memory for test_results_t");
		RTMB_printf(stderr, " in timer_config\n");
		targs->ret = ERROR;
		return (void *) NULL;
	}

	done = FALSE;
	evt_dispatch_sub_iterations = MIN_SUB_ITERATION;
	evt_dispatch_major_iterations = bmtest -> _iterations;

	clock_accuracy = get_min_exec_time(res_table);

	RTMB_verbose_printf(stdout, cmd_args, 1,
	        "\nTest Report for event_dispatch test:\n");
	RTMB_verbose_printf(stdout, cmd_args, 1,
	        "====================================\n");

	RTMB_verbose_printf(stdout, cmd_args, 1,
	        "\nevent_dispatch: Total number of iterations = %d\n\n",
	        new_iters);

	retry_number = 0;
	retry_status = UNACCEPTABLE_DEVIATIONS;

	while (!done) {
		run_status = run_event_dispatch_test(&sender_thread,
		        &receiver_thread, result, evt_dispatch_sub_iterations,
		        evt_dispatch_major_iterations);

		if (run_status == ERROR) {
			abort();
		} else if (IS_EXEC_TIME_GRT_THAN_CLK(result, clock_accuracy)) {
			print_flag = TRUE;
			rc = check_pass_criteria(result, cmd_args, bmtest, 0);

			if (rc == SUCCESS) {
				retry_status = ACCEPTABLE_DEVIATIONS;
				done = TRUE;
				break;
			} else {
				if (++retry_number == bmtest ->_threshold) {
					RTMB_printf(stderr,
					        "event_dispatch: exceeded"
						        " maximum attempts \n");
					break;
				}
			}
		}

		if (print_flag == TRUE) {
			RTMB_verbose_printf(stdout, cmd_args, 1,
			        "\nevent_dispatch: Retrying test ");
			RTMB_verbose_printf(stdout, cmd_args, 1,
			        "with bigger work quantum to get "
				        "lesser variance...\n");
		}

		free_chain(result, 0);
		evt_dispatch_sub_iterations *= MULTIPLIER_FOR_SUB_ITER;
	}

	add_result_to_result_table2(res_table, result, EVENT_DISPATCH_TEST,
	        bmtest);

	fflush(stderr);
	fflush(stdout);
	targs->ret = SUCCESS;
	free(receiver_end_time);
	free(sender_start_time);

	return (void *) NULL;
}

int run_event_dispatch_test(pthread_t *signal_sender,
        pthread_t *signal_receiver, test_results_t* result,
        int sub_iterations_to_set, int major_iterations_to_set) {
	int i;
	initialize_event_dispatch();

	receiver_end_time = calloc(sizeof(long long),
	        evt_dispatch_major_iterations);
	sender_start_time = calloc(sizeof(long long),
	        evt_dispatch_major_iterations);

	if (receiver_end_time == NULL) {
		RTMB_verbose_printf(stdout, cmd_args, 1,
		        " unable to allocate memory \n");
		return ERROR;
	}

	if (sender_start_time == NULL) {
		RTMB_verbose_printf(stdout, cmd_args, 1,
		        " unable to allocate memory for sender_start_time \n");
		free(receiver_end_time);
		return ERROR;
	}

	start_thread(signal_sender, signal_sender_start, NULL);
	start_thread(signal_receiver, signal_receiver_start, NULL);

	pthread_join(*signal_sender, NULL);
	pthread_join(*signal_receiver, NULL);

	for (i = 0; i < evt_dispatch_major_iterations; i++) {
		long long end = receiver_end_time[i];
		long long start = sender_start_time[i];
		long long time_diff;

		time_diff = (end - start);

		RTMB_verbose_printf(stdout, cmd_args, 2,
		        "event_dispatch: Difference between end and "
			        "start times = %.3f us \n", MICROSEC(time_diff));

		/* Now add the time diff to the table */
		if (add_entry(result, time_diff, 0) == ERROR) {
			RTMB_printf(stderr, " Unable to add time"
				" diff in EventDispatch @ "
				"%s:%d\n", __FILE__,__LINE__);
			return ERROR;
		}
	}

	cleanup_event_dispatch();
	return SUCCESS;
}
