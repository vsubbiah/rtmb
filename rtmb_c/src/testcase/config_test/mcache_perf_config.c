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
#include <array_operns.h>

/*
 * Name    : mcache_perf_config ()
 * Desc    : Configures benchmark with mcache perf rate on the system.
 * Args    : args - Test args.
 * Returns : NULL
 */
void *mcache_perf_config(void *args) {
	/* Get the args passed from parent thread */
	thread_args_t *targs = (thread_args_t *) args;
	cmd_line_args *cmd_args = targs->cmd_args;
	benchmark_test *bmtest = targs->bmtest;
	result_table_t *res_table = targs->res_table;
	test_results_t *result = NULL;

	/* Other Local variables of this function */
	char *m = NULL;
	int loop = 0, done = 0, count = 0;
	int num = DEF_ARRAY_SIZE;
	struct timespec start_time, end_time;
	long long time_diff;
	char retry_status = UNACCEPTABLE_DEVIATIONS;
	double clock_accuracy;
	int new_iters = bmtest->_iterations;
	char print_flag = FALSE;

	clock_accuracy = get_min_exec_time(res_table);

	result = create_test_result(1, new_iters);
	if (result == NULL) {
		RTMB_printf(stderr,
		        "ERROR: Cannot allocate memory for test_results_t");
		RTMB_printf(stderr, " in mcache_perf_config()\n");
		abort();
	}

	strcpy((char *) result->desc,
	        (char *) "Memory cache configuration test");
	RTMB_verbose_printf(stdout, cmd_args, 1, "\nTest Report for %s:\n",
	        (char*) &result->desc);
	RTMB_verbose_printf(stdout, cmd_args, 1,
	        "=====================================================\n");

	RTMB_verbose_printf(stdout, cmd_args, 1,
	        "\nmcache_perf_config: Total number of iterations = %d\n\n",
	        new_iters);

	while (!done) {
		for (loop = 0; loop < new_iters; loop++) {
			m = (char *) malloc(num);
			if (m == NULL) {
				RTMB_printf(stdout,
				        "**PANIC** malloc failed in "
					        "mcache_perf_config()\n");
				abort();
			}
			memset(m, 0, num);

			if (get_cur_time(&start_time) == ERROR) {
				free_chain(result, 0);
				targs->ret = ERROR;
				return (void *) NULL;
			}
			if (do_array_opern(m, num) == ERROR) {
				free_chain(result, 0);
				targs->ret = ERROR;
				return (void *) NULL;
			}

			if (get_cur_time(&end_time) == ERROR) {
				free_chain(result, 0);
				targs->ret = ERROR;
				return (void *) NULL;
			}
			if (m != (char *) NULL) {
				free(m);
			}
			/* Get the time difference of start and end times */
			time_diff = get_time_diff(start_time, end_time);
			RTMB_verbose_printf(stdout, cmd_args, 2,
			        "mcache_perf_config: Difference between end "
				        "and start times  = %.3f us \n",
			MICROSEC(time_diff));

			/* Now add the time diff to the table */
			add_entry(result, time_diff, 0);
		}

		if (IS_EXEC_TIME_GRT_THAN_CLK(result, clock_accuracy)) {
			print_flag = TRUE;

			/* Check against the input parameters for this test */
			if (check_pass_criteria(result, cmd_args, bmtest, 0)
			        == SUCCESS) {
				/* mcache perf is determined*/
				retry_status = ACCEPTABLE_DEVIATIONS;
				done = 1;
				break;
			} else {

				/*If we have completed, return error*/
				if (++count == bmtest->_threshold) {
					RTMB_printf(stderr,
					        "mcache_perf_config: exceeded "
						        "maximum attempts \n");
					break;
				}
			}
		}

		if (print_flag == TRUE) {
			RTMB_verbose_printf(stdout, cmd_args, 1,
			        "\nmcache_perf_config: Retrying test ");
			RTMB_verbose_printf(stdout, cmd_args, 1,
			        " with bigger work quantum to get"
				        " lesser variance...\n");
		}

		/*
		 * measured times are not consistent.
		 * so retry with larger array size
		 */
		free_chain(result, 0);
		num = num * MULTIPLIER_FOR_SUB_ITER;
	}

	result->opern_amount = num;

	add_result_to_result_table2(res_table, result, MCACHE_CONFIG, bmtest);

	fflush(stderr);
	fflush(stdout);
	targs->ret = SUCCESS;
	return (void *) NULL;
}
