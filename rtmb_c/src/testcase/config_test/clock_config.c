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

char run_clock_test(int num, int new_iters, test_results_t* result,
        cmd_line_args * cmd_args, benchmark_test* bm_test);

/*
 * Name    : clock_config()
 * Desc    : Configures the system clock
 * Args    : args - Test args
 * Returns : NULL
 */
void *clock_config(void *args) {
	/* Get the args passed from parent thread */
	thread_args_t *targs = (thread_args_t *) args;
	cmd_line_args *cmd_args = targs->cmd_args;
	benchmark_test *bmtest = targs->bmtest;
	result_table_t *res_table = targs->res_table;
	test_results_t *result = NULL;
	test_results_t *passed_result;
	/* Other Local variables of this function */

	int num = DEF_SZ_CLK_CONF;
	int new_iters = bmtest->_iterations;

	result = create_test_result(1, new_iters);

	passed_result = create_test_result(1, new_iters);

	if (result == NULL || passed_result == NULL) {
		RTMB_printf(stderr,
		        "ERROR: Cannot allocate memory for test_results_t");
		RTMB_printf(stderr, " in clock_config()\n");
		abort();
	}

	strcpy((char *) result->desc, (char *) "Clock configuration test");
	RTMB_verbose_printf(stdout, cmd_args, 1, "\nTest Report for %s:\n",
	        (char*) &result->desc);
	RTMB_verbose_printf(stdout, cmd_args, 1,
	        "=========================================\n",
	        (char*) &result->desc);
	RTMB_verbose_printf(stdout, cmd_args, 1,
	        "\nclock_config: Total number of iterations = %d\n\n",
	        new_iters);

	run_clock_test(num, new_iters, result, cmd_args,
		        bmtest);

	result->opern_amount = num;

	RTMB_verbose_printf(stdout, cmd_args, 1,
	        "clock_config: Benchmark will be configured ");
	RTMB_verbose_printf(stdout, cmd_args, 1,
	        "to expect the clock to be accurate to ");
	RTMB_verbose_printf(stdout, cmd_args, 1, "%.3f us\n",
	        MICROSEC(result->mediantime));

	add_result_to_result_table2(res_table, result, SYSCLOCK_CONFIG, bmtest);

	fflush(stderr);
	fflush(stdout);

	targs->ret = SUCCESS;
	return (void *) NULL;
}

char run_clock_test(int num, int new_iters, test_results_t* result,
        cmd_line_args * cmd_args, benchmark_test* bmtest) {
	int i;
	struct timespec start_time, end_time;
	long long time_diff;

	RTMB_verbose_printf(stdout, cmd_args, 3, "Task array size = %d\n",
	        num);

	for (i = 0; i < new_iters; i++) {
		get_cur_time(&start_time);
		get_cur_time(&end_time);

		time_diff = get_time_diff(start_time, end_time);

		RTMB_verbose_printf(stdout, cmd_args, 2,
		        "clock_config: Difference between end "
			        "and start times "
			        "= %.3f us \n", MICROSEC(time_diff));

		/* Add this to the table */
		add_entry(result, time_diff, 0);
	}

	/* Check against the input parameters for this test */
	check_pass_criteria(result, cmd_args, bmtest, 0);

	return UNACCEPTABLE_DEVIATIONS;
}
