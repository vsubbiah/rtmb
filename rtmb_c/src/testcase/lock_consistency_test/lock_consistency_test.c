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
 
#include <lock_consistency_test.h>

static cmd_line_args* cmd_args; /* stores cmd line args */
static double clock_accuracy;
static benchmark_test* bmtest;

/* initial value for the sub iteration */
#define SUB_ITERATIONS 2

/*
 * Name     : lock_consistency()
 * Desc     : Tests if higher priority threads preempt lower priority threads.
 * Args     : args - Test arguments passed from harness.
 * Returns  : NULL.
 */
void *lock_consistency(void *args) {
	int err_flag = SUCCESS;
	/* Get the args passed from parent thread */
	thread_args_t *targs = (thread_args_t *) args;
	cmd_args = targs->cmd_args;
	bmtest = targs->bmtest;
	result_table_t *res_table = targs->res_table;
	pthread_t high_priority_thrd;
	test_results_t *result_high_priority = NULL;
	char test_status = UNACCEPTABLE_DEVIATIONS;

	/* Locals */
	thr_args_t *high_prio_args = NULL;
	pthread_attr_t *high_prio_attr = NULL;
	int new_iters = bmtest->_iterations;

	RTMB_verbose_printf(stdout, cmd_args, 1,
	        "\nTest Report for lock_consistency test:\n");
	RTMB_verbose_printf(stdout, cmd_args, 1,
	        "======================================\n");

	RTMB_verbose_printf(stdout, cmd_args, 1,
	        "\nlock_consistency: Total number of iterations = %d\n\n",
	        new_iters);

	clock_accuracy = get_min_exec_time(res_table);

	/* Alloc memory for test_results */
	result_high_priority = create_test_result(1, new_iters);
	if (result_high_priority == NULL) {
		RTMB_printf(stderr,
		        "ERROR: malloc() failed in lock_consistency()\n");
		abort();
	}

	high_prio_attr = (pthread_attr_t *) malloc(sizeof(pthread_attr_t));
	high_prio_args = (thr_args_t *) malloc(sizeof(thr_args_t));

	if (high_prio_args == NULL || high_prio_attr == NULL) {
		RTMB_printf(stderr,
		        "ERROR: malloc() failed in lock_consistency()\n");
		abort();
	}

	/* Create and start a high priority threads */
	err_flag = create_start_threads(&high_priority_thrd, high_prio_attr,
	        high_prio_args, result_high_priority, new_iters);

	if (err_flag == ERROR) {
		RTMB_printf(stderr, "ERROR: Thread creation failed in "
			"lock_consistency()\n");
		abort();
	}

	/* wait for thread to finish its execution */
	pthread_join(high_priority_thrd, NULL);

	cleanup(high_prio_args, high_prio_attr);

	if (result_high_priority -> rc == SUCCESS) {
		test_status = ACCEPTABLE_DEVIATIONS;
	} else {
		test_status = UNACCEPTABLE_DEVIATIONS;
	}

	add_result_to_result_table2(res_table, result_high_priority,
	        LK_CONSISTENCY_TEST, bmtest);

	targs -> ret = SUCCESS;
	fflush(stderr);
	fflush(stdout);
	return (void *) NULL;
}

/*
 * Name     : cleanup()
 * Desc     : free()s malloced memory during the test
 * Args     : high_prio_args - thread args.
 *	      high_prio_attr - thread priorities.
 * Returns  : void
 */
void cleanup(thr_args_t* high_prio_args, pthread_attr_t * high_prio_attr) {
	if (high_prio_attr != NULL) {
		pthread_attr_destroy(high_prio_attr);
		free(high_prio_attr);
	}
	if (high_prio_args != NULL) {
		free(high_prio_args);
	}
}

/*
 * Name    : create_start_threads()
 * Desc    : Creates and starts the threads required for the test
 * Args    : args -
 * 		   1. thread attribute
 *		   2. attr
 * 		   3. thread argument structure
 *		   4. Result structure
 *		   5. Amount of work to do ( for add numbers)
 *		   6. Number of online cpus
 *		   7. Number of times to call the routine to  add numbers.
 * Returns : 0 on success and -1 on failure.
 */
int create_start_threads(pthread_t *thrd, pthread_attr_t *attr,
        thr_args_t *args, test_results_t *result, int iterations) {
	pthread_attr_init(attr);
	memset(args, 0, sizeof(thr_args_t));
	args->iterations = iterations;
	args->cmd_args = cmd_args;
	args->attr = attr;
	args->policy = SCHED_FIFO;
	args->results = result;
	int priority = HIGH_PRIO_VAL;

	if (set_pthreadattr_sched_param(args->attr, args->policy, priority)
	        == ERROR) {
		return ERROR;
	}

	if (pthread_create(thrd, args->attr, run_lock_consistency_test,
	        (void *) args) == ERROR) {
		perror("create_start_threads");
		RTMB_printf(stderr,
		        "create_start_threads: pthread_create() failed\n");
		return ERROR;
	}

	return SUCCESS;
}

/*
 * Name    : run_lock_consistency_test()
 * Desc    : Start method for a thread.
 * Args    : args - Thread arguments.
 * Returns : NULL
 */
void *run_lock_consistency_test(void *args) {
	thr_args_t *targs = (thr_args_t *) args;
	int new_iters = targs->iterations;
	test_results_t *results = targs->results;

	struct timespec end_time, start_time;
	int i = 0;
	struct sched_param param;
	int policy = -2;
	int retry_number = 0;
	int done = FALSE;
	int j, sub_iters = SUB_ITERATIONS;
	long long time_diff;
	pthread_mutex_t lock;
	int test_status = ERROR;
	char print_flag = FALSE;

	if (get_sched_param(pthread_self(), &policy, &param) != SUCCESS) {
		RTMB_printf(stderr, " unable to get policy and priority and"
			" priority of thread \n");
		policy = -1;
		param.sched_priority = -1;
		/* Assert in the next line will abort. We have a hard failure */
	}

	assert((param.sched_priority == HIGH_PRIO_VAL)
	        && (policy == SCHED_FIFO));

	retry_number = 0;
	done = FALSE;

	while (!done) {
		pthread_mutex_init(&lock, NULL);

		for (i = 0; i < new_iters; i++) {
			get_cur_time(&start_time);

			for (j = 0; j < sub_iters; j++) {
				pthread_mutex_lock(&lock);
				pthread_mutex_unlock(&lock);
			}

			get_cur_time(&end_time);
			time_diff = get_time_diff(start_time, end_time);

			RTMB_verbose_printf(stdout, cmd_args, 2,
			        "lock_consistency: Difference between end"
				        " and start times = %.3f us \n",
				        MICROSEC(time_diff));

			/* Add time_diff to the result structure*/
			add_entry(results, time_diff, 0);
		}

		if (IS_EXEC_TIME_GRT_THAN_CLK(results, clock_accuracy)) {
			print_flag = TRUE;
			test_status = check_pass_criteria(results, cmd_args,
			        bmtest, 0);

			if (test_status == ERROR) {
				if (++retry_number == bmtest ->_threshold) {
					RTMB_printf(stderr,
					        "lock_consistency: exceeded"
						        " maximum attempts \n");
					break;
				}
			} else {
				done = TRUE;
				break;
			}
		}

		if (print_flag == TRUE) {
			RTMB_verbose_printf(stdout, cmd_args, 1,
			        "\nlock_consistency: Retrying test");
			RTMB_verbose_printf(stdout, cmd_args, 1,
			        " with bigger work quantum to get"
				        " lesser variance...\n");
		}

		/*
		 * measured times are not accurate enough,
		 * hence retry.
		 */
		pthread_mutex_destroy(&lock);
		sub_iters *= MULTIPLIER_FOR_SUB_ITER;
		free_chain(results, 0);
	}

	return (void *) NULL;
}
