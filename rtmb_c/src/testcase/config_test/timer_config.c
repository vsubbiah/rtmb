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
 
#include <config_test.h>

void run_timer_test(int num, int iters, test_results_t *result,
        cmd_line_args *cmd_args, benchmark_test* bmtest) {
	int loop;

	struct timespec req_time, start_time, end_time;
	long long time_diff;

	req_time.tv_nsec = num % 1000000000LL;
	req_time.tv_sec = num / 1000000000LL;

	for (loop = 0; loop < iters; loop++) {
		if (get_cur_time(&start_time) == ERROR) {
			abort();
		}

		if (nanosleep(&req_time, NULL) != 0) {
			RTMB_printf(stderr, " nanosleep failed.. Exiting \n");
			abort();
		}

		if (get_cur_time(&end_time) == ERROR) {
			abort();
		}

		/* Get the time for which we actually slept in nsecs */
		time_diff = get_time_diff(start_time, end_time);
		RTMB_verbose_printf(stdout, cmd_args, 2,
		        "timer_config: Difference between end and start"
			        " times = %.3f us \n", MICROSEC(time_diff));

		/*
		 * We would have definitely slept slightly more than
		 * NANOSLEEP_TIME.
		 */
		add_entry(result, time_diff, 0);
	}
}
/*
 * Name    : timer_config()
 * Desc    : Determines accuracy of the timer.
 * Args    : args - Arguments to the test.
 * Returns : NULL
 */
void *timer_config(void *args) {
	/* Get the args passed from parent thread */
	thread_args_t *targs = (thread_args_t *) args;
	cmd_line_args *cmd_args = targs->cmd_args;
	benchmark_test *bmtest = targs->bmtest;
	result_table_t *res_table = targs->res_table;
	test_results_t *result = NULL;
	double clock_accuracy;
	int start_quantum, stop_quantum, mid_quantum;
	test_results_t *passed_result;
	char print_flag = FALSE;

	/* Other variables local to this function */
	int done = 0, count = 0;
	int num = NANOSLEEP_TIME;
	int prev_num = 0;
	int new_iters = bmtest->_iterations;
	double saved_percentage;
	double saved_range;
	double change_percent = 0;
	int test_pass;

	/*
	 * fill the rt structures with correct percentage
	 * and range for config tests.
	 * Since the same rt structure is used by all tests
	 * do not forget to retain the old values for percentages
	 * and values when the code returns from this function
	 */

	if (get_acceptable_percent(bmtest, cmd_args) < ACCEPTABLE_PCT_CFG_TEST
	        || get_range_percent(bmtest, cmd_args)
	                > ACCEPTABLE_RANGE_CFG_TEST) {
		change_percent = 1;
	} else {
		change_percent = 0;
	}

	if (change_percent) {
		fill_acceptable_percentages(bmtest, cmd_args,
		        ACCEPTABLE_PCT_CFG_TEST, ACCEPTABLE_RANGE_CFG_TEST,
		        &saved_percentage, &saved_range);
	}

	result = create_test_result(1, new_iters);
	passed_result = create_test_result(1, new_iters);
	if (result == NULL || passed_result == NULL) {
		RTMB_printf(stderr,
		        "ERROR: Cannot allocate memory for test_results_t");
		RTMB_printf(stderr, " in timer_config\n");
		abort();
	}

	strcpy((char *) result->desc, (char *) "Timer configuration test");
	RTMB_verbose_printf(stdout, cmd_args, 1, "\nTest Report for %s:\n",
	        (char*) &result->desc);
	RTMB_verbose_printf(stdout, cmd_args, 1,
	        "=========================================\n");
	RTMB_verbose_printf(stdout, cmd_args, 1,
	        "\ntimer_config : Total number of iterations = %d\n\n",
	        new_iters);

	clock_accuracy = get_min_exec_time(res_table);

	while (!done) {
		RTMB_verbose_printf(stdout, cmd_args, 3,
		        "timer_config: Requested sleep() duration: %d ns\n",
		        num);

		run_timer_test(num, new_iters, result, cmd_args, bmtest);

		if (IS_EXEC_TIME_GRT_THAN_CLK(result, clock_accuracy)) {
			print_flag = TRUE;
			if (check_pass_criteria(result, cmd_args, bmtest, 0)
			        == SUCCESS) {
				/*
				 * Std deviation looks good,
				 * accuracy of timer is determined
				 */
				test_pass = ACCEPTABLE_DEVIATIONS;
				done = 1;
				break;
			} else {
				count++;
				/* Return error if we have tried enough */
				if (count == bmtest->_threshold) {
					RTMB_printf(stderr,
					        "timer_config: exceeded "
						        "maximum attempts \n");
					test_pass = UNACCEPTABLE_DEVIATIONS;
					done = 1;
					break;
				}

			}
		}

		/*
		 * Std dev is not accurate enough,
		 * so retry with larger sleep time
		 */
		free_chain(result, 0);
		prev_num = num;
		num = num * MULTIPLIER_FOR_SUB_ITER;

		if (print_flag) {
			RTMB_verbose_printf(stdout, cmd_args, 1,
			        "\ntimer_config: Retrying test ");
			RTMB_verbose_printf(stdout, cmd_args, 1,
			        "with bigger work quantum to get "
				        "lesser variance...\n");
		}
	}

	result->opern_amount = num;

	if (test_pass == ACCEPTABLE_DEVIATIONS) {
		/*
		 * Perform binary search and find out least quantum of work
		 * that needs to be done in order to get the test pass.
		 */
		start_quantum = prev_num;
		stop_quantum = num;

		clone_results(passed_result, result);

		/*
		 * Run while loop
		 * for all possible quantums
		 */
		while (start_quantum <= stop_quantum) {
			char binary_search_test_pass = FALSE;
			free_chain(result, 0);
			mid_quantum = (start_quantum + stop_quantum) / 2;

			RTMB_verbose_printf(
			        stdout,
			        cmd_args,
			        3,
			        " start_quantum = %d mid_quantum = %d stop_quantum = %d \n",
			        start_quantum, mid_quantum, stop_quantum);

			run_timer_test(mid_quantum, new_iters, result,
			        cmd_args, bmtest);

			if (IS_EXEC_TIME_GRT_THAN_CLK(result, clock_accuracy)
			        && (check_pass_criteria(result, cmd_args,
			                bmtest, 0) == SUCCESS)) {
				binary_search_test_pass = ACCEPTABLE_DEVIATIONS;
			} else {
				binary_search_test_pass
				        = UNACCEPTABLE_DEVIATIONS;
			}

			if (binary_search_test_pass == UNACCEPTABLE_DEVIATIONS) {
				start_quantum = mid_quantum + 1;
			} else if (binary_search_test_pass
			        == ACCEPTABLE_DEVIATIONS) {
				result -> opern_amount = mid_quantum;
				clone_results(passed_result, result);
				stop_quantum = mid_quantum - 1;
			}
		}

		compute_std_time(passed_result, 0);

		delete_test_result(result);
		result = passed_result;
	}

	RTMB_verbose_printf(stdout, cmd_args, 1,
	        "timer_config: Benchmark will be configured ");
	RTMB_verbose_printf(stdout, cmd_args, 1,
	        "to expect the timer to be accurate to ");
	RTMB_verbose_printf(stdout, cmd_args, 1, "%.3f us\n",
		MICROSEC(result->mediantime));

	add_result_to_result_table2(res_table, result, SYSTIMER_CONFIG, bmtest);

	if (change_percent) {
		/* revert the percentages and ranges to original values */
		fill_acceptable_percentages(bmtest, cmd_args, saved_percentage,
		        saved_range, NULL, NULL);
	}

	fflush(stderr);
	fflush(stdout);
	targs->ret = SUCCESS;
	return (void *) NULL;
}
