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

int disk_io_write(test_results_t* result, int iters, cmd_line_args * cmd_args,
        benchmark_test* bmtest, result_table_t* res_table) {
	FILE *write_fp = NULL;
	long long time_diff;
	int block_size;
	int done, loop, j;

	int count = 0;
	char *write_buf = NULL;
	struct timespec start_time, end_time;
	double clock_accuracy;
	int retry_status = UNACCEPTABLE_DEVIATIONS;
	char print_flag = FALSE;
	int sub_iters = 1;

	block_size = get_block_size();

	assert( block_size> 0 );

	clock_accuracy = get_min_exec_time(res_table);
	RTMB_verbose_printf(stdout, cmd_args, 1,
	        "\nTest report for disk I/O write configuration:\n");

	RTMB_verbose_printf(stdout, cmd_args, 1,
	        "=================================================\n");
	RTMB_verbose_printf(stdout, cmd_args, 1,
	        "\ndisk_io_write_config: Total number of iterations = %d\n\n",
	        iters);

	done = 0;
	while (!done) {
		int n;
		write_buf = calloc(1, block_size);

		if (write_buf == NULL)
			abort();

		for (loop = 0; loop < iters; loop++) {
			open_file_for_write(&write_fp);

			get_cur_time(&start_time);

			for (j = 0; j < sub_iters; j++) {
				if ((n = fwrite(write_buf, sizeof(char),
				        block_size, write_fp)) != block_size) {
					perror("fwrite:");
					abort();
				}
			}

			get_cur_time(&end_time);

			/* Get the time difference of start and end times */
			time_diff = get_time_diff(start_time, end_time);
			RTMB_verbose_printf(stdout, cmd_args, 2,
			        "disk_io_write_config: Difference between end"
				        " and start times = %.3f us\n",
			        MICROSEC(time_diff));

			add_entry(result, time_diff, 0);
			fclose(write_fp);
		}

		if (IS_EXEC_TIME_GRT_THAN_CLK(result, clock_accuracy)) {
			print_flag = TRUE;
			/* Check against the input parameters for this test */
			if (check_pass_criteria(result, cmd_args, bmtest, 0)
			        == SUCCESS) {
				/* disk IO rate is determined*/
				done = 1;
				retry_status = ACCEPTABLE_DEVIATIONS;
				break;
			} else {
				if (++count == bmtest->_threshold) {
					RTMB_printf(stderr,
					        "disk_io_write_config: exceeded"
						        " maximum attempts \n");
					break;
				}

			}
		}

		if (print_flag == TRUE) {
			RTMB_verbose_printf(stdout, cmd_args, 1,
			        "\ndisk_io_write_config: Retrying "
				        "test");
			RTMB_verbose_printf(stdout, cmd_args, 1,
			        " with bigger work quantum to get"
				        " lesser variance...\n");
		}

		/*
		 * measured times are not consistent. so retry
		 */
		free_chain(result, 0);
		sub_iters *= MULTIPLIER_FOR_SUB_ITER;
		free(write_buf);
	}

	result->opern_amount = block_size * sub_iters;

	return retry_status;
}

/*
 * Name    : disk_io_write_config()
 * Desc    : Configures the disk IO write rates.
 * Args    : args - Test args
 * Returns : NULL
 */
void *disk_io_write_config(void *args) {
	/* Get the args passed from parent thread */
	thread_args_t *targs = (thread_args_t *) args;
	cmd_line_args *cmd_args = targs->cmd_args;
	benchmark_test *bmtest = targs->bmtest;
	result_table_t *res_table = targs->res_table;
	test_results_t *write_conf_result = NULL;
	int test_status;

	/* Make reads and writes of 1k initially */
	int new_iters = bmtest->_iterations;

	disk_io_init();
	write_conf_result = create_test_result(1, new_iters);

	RTMB_verbose_printf(stdout, cmd_args, 3,
	        "disk_io_write_config: Measuring the write performance...\n");

	test_status = disk_io_write(write_conf_result, new_iters, cmd_args,
	        bmtest, res_table);

	add_result_to_result_table2(res_table, write_conf_result,
	        DISKIO_WRITE_CONFIG, bmtest);

	disk_io_cleanup();
	targs->ret = SUCCESS;
	return (void *) NULL;
}
