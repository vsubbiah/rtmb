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
#include <string.h>
#include <io_opern.h>
#include <assert.h>

char disk_io_read(test_results_t* result, int iters, cmd_line_args * cmd_args,
        benchmark_test* bmtest, result_table_t* res_table) {
	FILE *read_fp = NULL;
	long long time_diff;
	int block_size = 0;
	int done, loop, j;
	double clock_accuracy;
	char retry_status = UNACCEPTABLE_DEVIATIONS;
	int sub_iters = DEFAULT_SUBITERS;

	int count = 0;
	char *_read_buf = NULL;
	char print_flag = FALSE;
	struct timespec start_time, end_time;

	block_size = get_block_size();

	assert(block_size> 0);

	clock_accuracy = get_min_exec_time(res_table);

	RTMB_verbose_printf(stdout, cmd_args, 1,
	        "\nTest Report for disk I/O read configuration:\n");

	RTMB_verbose_printf(stdout, cmd_args, 1,
	        "=================================================\n");
	RTMB_verbose_printf(stdout, cmd_args, 1,
	        "\ndisk_io_config: Total number of iterations = %d\n\n", iters);

	done = 0;
	while (!done) {
		int n;
		_read_buf = calloc(1, block_size);

		if (_read_buf == NULL) {
			RTMB_printf(stderr,
			        "calloc() failed in disk_io_read_config()\n");
			abort();
		}

		/*Make sure there is data in the file before attempting a read*/
		setup_file_for_read(block_size * sub_iters, 1);

		for (loop = 0; loop < iters; loop++) {
			open_file_for_read(&read_fp);

			if (get_cur_time(&start_time) == ERROR) {
				abort();
			}

			for (j = 0; j < sub_iters; j++) {
				if ((n = fread(_read_buf, sizeof(char),
				        block_size, read_fp)) != block_size) {
					perror("fwrite:");
					abort();
				}
			}

			if (get_cur_time(&end_time) == ERROR) {
				abort();
			}

			/* Get the time difference of start and end times */
			time_diff = get_time_diff(start_time, end_time);
			RTMB_verbose_printf(stdout, cmd_args, 2,
			        "disk_io_read_config: Difference between end"
				        " and start times = %.3f us\n",
			        MICROSEC(time_diff));

			add_entry(result, time_diff, 0);
			fclose(read_fp);
		}

		if (IS_EXEC_TIME_GRT_THAN_CLK(result, clock_accuracy)) {
			print_flag = TRUE;
			if (check_pass_criteria(result, cmd_args, bmtest, 0)
			        == SUCCESS) {
				/*
				 * test passed,
				 * disk IO rate is determined
				 */
				retry_status = ACCEPTABLE_DEVIATIONS;
				done = 1;
				break;
			} else {
				/*If we have completed, return error*/
				if (++count == bmtest->_threshold) {
					RTMB_printf(stderr,
					        "disk_io_read_config: exceeded"
						        " maximum attempts \n");
					break;
				}

			}
		}

		if (print_flag == TRUE) {
			RTMB_verbose_printf(stdout, cmd_args, 1,
			        "\ndisk_io_read_config: Retrying test");
			RTMB_verbose_printf(stdout, cmd_args, 1,
			        " with bigger work quantum to get"
				        " lesser variance...\n");
		}

		/*
		 * measured times are not accurate enough,
		 * hence retry.
		 */
		free_chain(result, 0);
		sub_iters *= MULTIPLIER_FOR_SUB_ITER;
		free(_read_buf);
	}

	result->opern_amount = block_size * sub_iters;

	return retry_status;
}

/*
 * Name    : disk_io_read_config()
 * Desc    : Configures the disk IO read rates.
 * Args    : args - Test args
 * Returns : NULL
 */
void *disk_io_read_config(void *args) {
	/* Get the args passed from parent thread */
	thread_args_t *targs = (thread_args_t *) args;
	cmd_line_args *cmd_args = targs->cmd_args;
	benchmark_test *bmtest = targs->bmtest;
	result_table_t *res_table = targs->res_table;
	test_results_t *read_conf_result = NULL;
	int test_status;

	int new_iters = bmtest->_iterations;
	RTMB_verbose_printf(stdout, cmd_args, 3,
	        "disk_io_config: Measuring the read performance...\n");

	disk_io_init();
	read_conf_result = create_test_result(1, new_iters);

	test_status = disk_io_read(read_conf_result, new_iters, cmd_args,
	        bmtest, res_table);

	add_result_to_result_table2(res_table, read_conf_result,
	        DISKIO_READ_CONFIG, bmtest);

	disk_io_cleanup();
	targs->ret = SUCCESS;
	return (void *) NULL;
}
