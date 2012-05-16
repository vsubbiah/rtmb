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
#include <math_opern.h>

/*
 * Name    : float_rate_config()
 * Desc    : Configures benchmark for floating rate on the machine.
 * Args    : args - Test args.
 * Returns : NULL
 */
void *float_rate_config(void *args) {
	/* Get the args passed from parent thread */
	thread_args_t *targs = (thread_args_t *) args;
	cmd_line_args *cmd_args = targs->cmd_args;
	benchmark_test *bmtest = targs->bmtest;
	result_table_t *res_table = targs->res_table;
	test_results_t *result = NULL;
	int retry_status = UNACCEPTABLE_DEVIATIONS;
	char print_flag = FALSE;

	/* Other Local variables of this function */
	double clock_accuracy;
	int loop = 0, done = 0, count = 0;
	struct timespec start_time, end_time;
	long long time_diff;
	int num = 100;
	int new_iters = bmtest->_iterations;

	clock_accuracy = get_min_exec_time(res_table);

	result = create_test_result(1, new_iters);
	if (result == NULL) {
		RTMB_printf(stderr,
		        "ERROR: Cannot allocate memory for test_results_t");
		RTMB_printf(stderr, " in float_rate_config()\n");
		abort();
	}

	strcpy((char *) result->desc,
	        (char *) "Floating rate configuration test");
	RTMB_verbose_printf(stdout, cmd_args, 1, "\nTest Report for %s:\n",
	        (char*) &result->desc);
	RTMB_verbose_printf(stdout, cmd_args, 1,
	        "============================================================="
		        "========\n");
	RTMB_verbose_printf(stdout, cmd_args, 1,
	        "\nfloat_rate_config : Total number of iterations = %d\n\n",
	        new_iters);

	while (!done) {
		for (loop = 0; loop < new_iters; loop++) {
			if (get_cur_time(&start_time) == ERROR) {
				free_chain(result, 0);
				targs->ret = ERROR;
				return (void *) NULL;
			}
			if (do_simple_float_opern(num) == (double) ERROR) {
				free_chain(result, 0);
				targs->ret = ERROR;
				return (void *) NULL;
			}
			if (get_cur_time(&end_time) == ERROR) {
				free_chain(result, 0);
				targs->ret = ERROR;
				return (void *) NULL;
			}
			/* Get the time difference of start and end times */
			time_diff = get_time_diff(start_time, end_time);
			RTMB_verbose_printf(stdout, cmd_args, 2,
			        "float_rate_config: Difference between end and"
				        " start times  = %.3f us\n", MICROSEC(
			                time_diff));

			/* Add this to the table */
			add_entry(result, time_diff, 0);
		}

		if (IS_EXEC_TIME_GRT_THAN_CLK(result, clock_accuracy)) {
			print_flag = TRUE;

			/* Check against the input parameters for this test */
			if (check_pass_criteria(result, cmd_args, bmtest, 0)
			        == SUCCESS) {
				/* floating rate is determined */
				done = 1;
				retry_status = ACCEPTABLE_DEVIATIONS;
				break;
			} else {
				if (++count == bmtest->_threshold) {
					RTMB_printf(stderr,
					        "float_rate_config: exceeded "
						        "maximum attempts\n");
					break;
				}

			}
		}

		if (print_flag == TRUE) {
			RTMB_verbose_printf(stdout, cmd_args, 1,
			        "\nfloat_rate_config: Retrying test ");
			RTMB_verbose_printf(stdout, cmd_args, 1,
			        " with bigger work quantum to get"
				        " lesser variance...\n");
		}

		/*
		 * measured times are not consistent.
		 * so retry with larger quantum.
		 */
		free_chain(result, 0);
		num = num * MULTIPLIER_FOR_SUB_ITER;
	}

	result->opern_amount = num;

	add_result_to_result_table2(res_table, result, FRATE_CONFIG, bmtest);

	fflush(stderr);
	fflush(stdout);
	targs->ret = SUCCESS;
	return (void *) NULL;
}
