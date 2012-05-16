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
 
#include <multi_thread_test.h>
#include <time.h>
#include <stdlib.h>
#include <math.h>

/* Globals */
int counter = 0;
static int online_cpus = 0;
static cmd_line_args* cmd_args; /* stores cmd line args */
static benchmark_test *bmtest;
static int sub_iters = DEFAULT_SUB_ITERATION; /* default sub iterations*/
static int iterations = 0;
static double clock_accuracy;

#define MAXINT 10000
#define SIZE 10
#define INITIALIZE_SEED 10000

void* sample_run(void *arg);

static vector_t* get_multi_thread_scores(test_results_t* tresult,
        test_results_t* consolidator, int n, benchmark_test* bmtest) {
	vector_t *result_vec;
	vector_t *score_vec;
	test_score_t *score;
	double parallelism;
	double product = 1;
	double sum = 0;
	int i = 0;
	result_vec = create_vector();

	for (i = 0; i <= n - 1; i++) {
		add_element(result_vec, &tresult[i]);
	}

	score_vec = generate_combined_score(result_vec,
	        bmtest ->_reference_score);
	score = get_element_at(score_vec, 0);

	for (i = 0; i < n - 1; i++) {
		product *= pow(tresult[i].mediantime, 1.0 / online_cpus);
		sum += tresult[i].mediantime;
	}

	parallelism = sum / (online_cpus * consolidator -> mediantime);
	score -> throughput_score = bmtest -> _reference_score / (product
	        / parallelism);
	return score_vec;
}

void fill_matrix(int matrix[SIZE][SIZE]) {
	int i, j;
	unsigned int seedp = 10000;

	for (i = 0; i < SIZE; i++)
		for (j = 0; j < SIZE; j++)
			matrix[i][j] = 1 + (int) (MAXINT * (rand_r(&seedp)
			        / (RAND_MAX + 1.0)));
}

/*
 * Name     : multi_thread()
 * Desc     : Tests if higher priority threads preempt lower priority threads.
 * Args     : args - Test arguments passed from harness.
 * Returns  : NULL.
 */
void *multi_thread(void *args) {
	pthread_t sample_run_thrd;

	/* Get the args passed from parent thread */
	thread_args_t *targs = (thread_args_t *) args;
	cmd_args = targs->cmd_args;
	bmtest = targs->bmtest;
	result_table_t *res_table = targs->res_table;
	targs->ret = SUCCESS;
	test_results_t *result_high_priority = NULL;
	test_results_t *result_consolidator = NULL;
	int retry_number = 0;
	char done = TRUE;
	char test_pass = UNACCEPTABLE_DEVIATIONS;

	pthread_attr_t high_prio_attr;
	int new_iters = bmtest->_iterations;
	int total_thrs = 0;
	int rc;
	vector_t* score_vec;

	iterations = new_iters;

	/* Get the total number of online CPUs on the system */
	online_cpus = get_num_online_cpus();

	/* DEBUG */
	if (online_cpus == 1) {
		RTMB_verbose_printf(stdout, cmd_args, 1,
		        "multi_thread: Test not done on uniprocessor systems\n");
		targs->ret = SUCCESS;
		return (void *) NULL;
	}

	RTMB_verbose_printf(stdout, cmd_args, 1,
	        "\nTest report for multi_thread test:\n");
	RTMB_verbose_printf(stdout, cmd_args, 1,
	        "==================================\n");

	RTMB_verbose_printf(stdout, cmd_args, 1,
	        "\nmulti_thread: Total number of iterations = %d\n\n",
	        new_iters);

	total_thrs = online_cpus + 1;

	/* Alloc memory for test_results */
	result_high_priority = create_test_result(1, new_iters);

	clock_accuracy = get_min_exec_time(res_table);

	pthread_attr_init(&high_prio_attr);

	rc = pthread_create(&sample_run_thrd, &high_prio_attr, sample_run,
	        result_high_priority);

	if (rc < 0) {
		RTMB_printf(stderr, "ERROR in thread creation\n");
		abort();
	}

	pthread_join(sample_run_thrd, NULL);

	free_chain(result_high_priority, 0);

	if (result_high_priority[0].rc == ERROR) {
		vector_t * result_vec = create_vector();
		test_score_t *test_score;

		if (result_vec == NULL) {
			RTMB_printf(stderr, " unable to allocate memory "
				". rtmb panic @ %s:%d\n", __FILE__,__LINE__);
			abort();
		}

		RTMB_verbose_printf(stderr, cmd_args, 1,
		        "multi_thread: Test failed\n");

		add_element(result_vec, &result_high_priority[0]);

		test_score = create_test_score1(result_vec);

		score_vec = create_vector();
		add_element(score_vec, test_score);
		test_pass = UNACCEPTABLE_DEVIATIONS;
		targs -> ret = ERROR;

		return NULL;
	}

	delete_test_result(result_high_priority);

	result_high_priority = create_test_result(total_thrs, new_iters);
	result_consolidator = create_test_result1(1, new_iters, total_thrs);

	if (result_high_priority == NULL || result_consolidator == NULL) {
		RTMB_printf(stderr,
		        "ERROR: malloc() failed in multi_thread()\n");
		abort();
	}

	done = FALSE;
	retry_number = 0;
	test_pass = UNACCEPTABLE_DEVIATIONS;

	while (!done && (retry_number < bmtest ->_threshold)) {
		rc = run_multi_thread_test(new_iters, sub_iters,
		        result_high_priority, result_consolidator, online_cpus,
		        total_thrs);

		if (rc == ERROR) {
			RTMB_verbose_printf(stdout, cmd_args, 1,
			        "\nmulti_thread: Retrying test");
			RTMB_verbose_printf(stdout, cmd_args, 1,
			        " with bigger work quantum to get"
				        " lesser variance...\n");

			done = FALSE;
			sub_iters *= MULTIPLIER_FOR_SUB_ITER;
			retry_number++;

			//TODO free chains
			free_chains(result_high_priority, total_thrs, 0);
			free_chain(result_consolidator, 0);
		} else {
			test_pass = ACCEPTABLE_DEVIATIONS;
			done = TRUE;
			break;
		}
	}

	if (retry_number == bmtest ->_threshold) {
		RTMB_printf(stderr, "multi_thread: exceeded"
			" maximum attempts \n");
	}

	score_vec = get_multi_thread_scores(result_high_priority,
	        result_consolidator, total_thrs, bmtest);

	add_result_to_result_table4(res_table, score_vec, MULTI_THREAD_TEST,
	        bmtest, test_pass);

	fflush(stderr);
	fflush(stdout);
	return (void *) NULL;
}

int run_multi_thread_test(int iters, int sub_iters,
        test_results_t* result_high_priority,
        test_results_t* result_consolidator, int online_cpus, int total_thrs) {
	int i;
	int start_index = 0, end_index = online_cpus - 1;
	thread_syncers_t* tsyncers = NULL;
	thread_id_t *thr_id;
	thread_syncers_t *parent_sema = NULL;
	int err_flag = SUCCESS;
	struct timespec wait_time;
	int ret = SUCCESS;

	/* Create and start "online_cpus" number of high priority threads */
	thr_args_t *high_prio_args = NULL;
	pthread_attr_t high_prio_attr;

	high_prio_args = (thr_args_t *) calloc(sizeof(thr_args_t), total_thrs);
	thr_id = (thread_id_t *) calloc(sizeof(thread_id_t), total_thrs);

	if (thr_id == NULL || high_prio_args == NULL) {
		RTMB_printf(stderr,
		        "ERROR: malloc() failed in multi_thread()\n");
		abort();
	}

	for (i = 0; i < total_thrs; i++) {
		result_high_priority[i].result_id = i;
	}

	/*
	 * Allocate memory for the mutex and cond vars that we use
	 *  during this test
	 */
	tsyncers = (thread_syncers_t *) calloc(1, sizeof(thread_syncers_t));
	if (tsyncers == NULL) {
		RTMB_printf(stderr,
		        "ERROR: malloc() failed in multi_thread()\n");
		abort();
	}

	pthread_mutex_init(&tsyncers->mutex, NULL);
	pthread_cond_init(&tsyncers->cond, NULL);

	parent_sema = (thread_syncers_t *) calloc(1, sizeof(thread_syncers_t));

	if (parent_sema == NULL) {
		RTMB_printf(stderr,
		        "ERROR: malloc() failed in multi_thread()\n");
		abort();
	}

	pthread_mutex_init(&parent_sema->mutex, NULL);
	pthread_cond_init(&parent_sema->cond, NULL);

	err_flag = create_multi_threads(HIGH_PRIO_VAL, high_prio_args,
	        result_high_priority, start_index, end_index, thr_id, tsyncers,
	        parent_sema);

	if (err_flag == ERROR) {
		RTMB_printf(stderr,
		        "ERROR: Thread creation failed in multi_thread()\n");
		abort();
	}

	wait_time.tv_sec = 1;
	wait_time.tv_nsec = 0;

	while (counter != (total_thrs - 1)) {
		pthread_mutex_lock(&parent_sema->mutex);
		pthread_cond_timedwait(&parent_sema->cond, &parent_sema->mutex,
		        &wait_time);
		pthread_mutex_unlock(&parent_sema->mutex);
	}

	pthread_mutex_lock(&tsyncers->mutex);
	pthread_cond_broadcast(&tsyncers->cond);
	pthread_mutex_unlock(&tsyncers->mutex);

	/* Parent thread waits for all threads to complete */
	while (counter != 0) {
		pthread_mutex_lock(&parent_sema->mutex);
		pthread_cond_wait(&parent_sema->cond, &parent_sema->mutex);
		pthread_mutex_unlock(&parent_sema->mutex);
	}

	/* Create and start "online_cpus" number of high priority threads */
	err_flag = create_multi_threads(HIGH_PRIO_VAL, high_prio_args,
	        result_high_priority, end_index + 1, end_index + 1, thr_id,
	        tsyncers, parent_sema);

	if (err_flag == ERROR) {
		RTMB_printf(stderr,
		        "ERROR: Thread creation failed in multi_thread()\n");
		abort();
	}

	for (i = 0; i < total_thrs; i++) {
		pthread_join(thr_id[i].tid, NULL );

		if (check_pass_criteria(&result_high_priority[i], cmd_args,
		        bmtest, 0) == ERROR) {
			/* test failed */
			RTMB_verbose_printf(stdout, cmd_args, 3,
			        "\n multi_thread: thread %d failed \n", i);
			ret = ERROR;
		} else {
			RTMB_verbose_printf(stdout, cmd_args, 3,
			        "\n multi_thread: thread %d passed \n ", i);
		}

		add_entry(result_consolidator,
		        result_high_priority[i].mediantime, 0);
	}

	if (ret == SUCCESS) {
		if (check_pass_criteria(result_consolidator, cmd_args, bmtest,
		        0) == ERROR) {
			ret = ERROR;
			RTMB_verbose_printf(stdout, cmd_args, 3,
			        "multi_thread: consolidated_result failed \n");
			ret = ERROR;
		} else {
			RTMB_verbose_printf(stdout, cmd_args, 3,
			        "multi_thread: consolidated_result passed \n");
		}
	}

	for (i = 0; i < total_thrs; i++) {
		char buf[30];
		sprintf(buf, " high priority thread #%d", i);
		set_test_result_name(&result_high_priority[i], buf);
	}

	cleanup(high_prio_args, &high_prio_attr, tsyncers, parent_sema, thr_id);
	return ret;
}

/*
 * Name     : cleanup()
 * Desc     : free()s malloced memory during the test
 * Args     : low_prio_args, med_prio_args and high_prio_args - thread args.
 *	      med_prio_attr, low_prio_attr and high_prio_attr - thread
 *	             priorities.
 * Returns  : void
 */
void cleanup(thr_args_t *high_prio_args, pthread_attr_t *high_prio_attr,
        thread_syncers_t* tsyncers, thread_syncers_t *parent_sema,
        thread_id_t* thr_id) {
	if (tsyncers != NULL) {
		pthread_mutex_destroy(&tsyncers->mutex);
		pthread_cond_destroy(&tsyncers->cond);
		free(tsyncers);
	}
	if (parent_sema != NULL) {
		pthread_mutex_destroy(&parent_sema->mutex);
		pthread_cond_destroy(&parent_sema->cond);
		free(parent_sema);
	}

	if (high_prio_args != NULL) {
		free(high_prio_args);
	}

	if (thr_id != NULL) {
		free(thr_id);
	}
}

/*
 * Name    : create_multi_threads()
 * Desc    : Creates and starts the threads required for the test
 * Args    : args -
 * 		   1. thread attribute
 *		   2. type ( high /medium /low )
 * 		   3. thread argument structure
 *		   4. Result structure
 *		   5. Amount of work to do ( for add numbers)
 *		   6. Number of online cpus
 *		   7. Number of times to call the routine to  add numbers.
 * Returns : 0 on success and -1 on failure.
 */
int create_multi_threads(int priority, thr_args_t *args_array,
        test_results_t *result, int start_index, int end_index,
        thread_id_t* thr_id, thread_syncers_t* tsyncers,
        thread_syncers_t* parent_sema) {

	int i;
	thr_args_t* args = NULL;

	pthread_attr_t attr;
	pthread_attr_init(&attr);

	if (set_pthreadattr_sched_param(&attr, SCHED_FIFO, priority) == ERROR) {
		return ERROR;
	}

	for (i = start_index; i <= end_index; i++) {
		args = &args_array[i];
		args -> cmd_args = cmd_args;
		args -> attr = &attr;
		args -> policy = SCHED_FIFO;
		args -> tsyncers = tsyncers;
		args -> parent_sema = parent_sema;
		args-> results = &result[i];

		if (pthread_create(&thr_id[i].tid, args->attr,
		        start_multi_thread_test, (void *) args) == ERROR) {
			perror("create_multi_threads");
			RTMB_printf(stderr,
			        "create_multi_threads: pthread_create() "
				        "failed\n");
			return ERROR;
		}
	}

	pthread_attr_destroy(&attr);
	return SUCCESS;
}

/*
 * Name    : lock_inc_counter()
 * Desc    : Increment the thread counter under protection of a lock.
 * Returns : void
 */
void lock_inc_counter(thread_syncers_t* tsyncers, thread_syncers_t* parent_sema) {
	/* Acquire the lock on the list */
	pthread_mutex_lock(&tsyncers->mutex);

	/* Increment the counter to state "self" was started */
	counter++;
	/*printf ("waiting thread id = %d\n", pthread_self());*/
	pthread_cond_wait(&tsyncers->cond, &tsyncers->mutex);

	/* Unlock and let other threads increment */
	pthread_mutex_unlock(&tsyncers->mutex);
}

/*
 * Name    : lock_dec_counter()
 * Desc    : Decrement the thread counter under protection of a lock.
 * Args    : void
 * Returns : void
 */
void lock_dec_counter(thread_syncers_t* tsyncers, thread_syncers_t* parent_sema) {
	/* Acquire the lock on the list */
	pthread_mutex_lock(&tsyncers->mutex);

	/* Decrement the counter to state "self" has ended */
	counter--;

	/* Notify mother of all threads who is waiting */
	if (counter == 0) {
		pthread_mutex_lock(&parent_sema->mutex);
		pthread_cond_signal(&parent_sema->cond);
		pthread_mutex_unlock(&parent_sema->mutex);
	}
	/* Unlock and let other threads decrement */
	pthread_mutex_unlock(&tsyncers->mutex);
}

/*
 * Name    : run_multi_thread_test()
 * Desc    : Start method for a thread.
 * Args    : args - Thread arguments.
 * Returns : NULL
 */
void *start_multi_thread_test(void *args) {
	thr_args_t *targs = (thr_args_t *) args;
	int new_iters = iterations;
	cmd_line_args *cmd_args = targs->cmd_args;
	test_results_t *results = targs->results;
	thread_syncers_t* tsyncers = targs -> tsyncers;
	thread_syncers_t* parent_sema = targs -> parent_sema;

	struct timespec end_time, start_time;
	long long diff_time = 0;
	int i = 0, j = 0, p = 0, q = 0, r = 0;
	struct sched_param param;
	int policy = -2;
	int matrixA[SIZE][SIZE], matrixB[SIZE][SIZE], matrixC[SIZE][SIZE], sum;
	fill_matrix(matrixA);
	fill_matrix(matrixB);

	if (get_sched_param(pthread_self(), &policy, &param) != SUCCESS) {
		RTMB_printf(stderr,
		        " unable to get policy and priority and priority of "
			        "thread \n");
		policy = -1;
		param.sched_priority = -1;
		/* Assert in the next line will abort. We have a hard failure */
	}

	assert((param.sched_priority == HIGH_PRIO_VAL)
	        && (policy == SCHED_FIFO));

	if (results -> result_id != online_cpus)
		/*
		 * Increment the counter with protection and wait
		 *  for all to start
		 */
		lock_inc_counter(tsyncers, parent_sema);

	for (i = 0; i < new_iters; i++) {
		/* Get the end time */
		if (get_cur_time(&start_time) == ERROR) {
			lock_dec_counter(tsyncers, parent_sema);
			results->rc = ERROR;
			return (void *) NULL;
		}

		for (j = 0; j < sub_iters; j++) {
			/* matrix multiplication code here */
			for (p = 0; p < SIZE; p++) {
				for (q = 0; q < SIZE; q++) {
					sum = 0;

					for (r = 0; r < SIZE; r++) {
						sum += matrixA[p][q]
						        * matrixB[q][r];
					}

					matrixC[p][q] = sum;
				}
			}
		}

		/* Get the end time */
		if (get_cur_time(&end_time) == ERROR) {
			lock_dec_counter(tsyncers, parent_sema);
			results->rc = ERROR;
			return (void *) NULL;
		}

		diff_time = get_time_diff(start_time, end_time);
		add_entry(results, diff_time, 0);
		RTMB_verbose_printf(stdout, cmd_args, 2,
		        "multi_thread: #%d Difference between end and start"
			        " times = %.3f us\n", results -> result_id,
			        MICROSEC(diff_time));
	}

	fflush(stderr);
	fflush(stdout);
	/* Add this into the test results*/

	/* Decrement the counter and return */
	if (results -> result_id != online_cpus)
		lock_dec_counter(tsyncers, parent_sema);
	results->rc = SUCCESS;
	return (void *) NULL;
}

/*
 * Name    : sample_run()
 * Desc    : does a sample run to determine number of sub iterations
 * Args    : pointer to test results structure
 * Returns : none.
 */
void* sample_run(void *arg) {
	test_results_t * result = (test_results_t *) arg;
	int i = 0, j = 0, p = 0, q = 0, r = 0;
	struct timespec start_time, end_time;
	long long diff_time;

	int done = 0;

	int matrixA[SIZE][SIZE], matrixB[SIZE][SIZE], matrixC[SIZE][SIZE], sum;
	fill_matrix(matrixA);
	fill_matrix(matrixB);

	while (!done) {
		for (i = 0; i < iterations; i++) {
			/* Get the end time */
			if (get_cur_time(&start_time) == ERROR) {
				result->rc = ERROR;
				return NULL;
			}

			for (j = 0; j < sub_iters; j++) {
				/* matrix multiplication code here */
				for (p = 0; p < SIZE; p++) {
					for (q = 0; q < SIZE; q++) {
						sum = 0;

						for (r = 0; r < SIZE; r++) {
							sum += matrixA[p][r]
							        * matrixB[r][q];
						}

						matrixC[p][q] = sum;
					}
				}
			}

			/* Get the end time */
			if (get_cur_time(&end_time) == ERROR) {
				result->rc = ERROR;
				return NULL;
			}

			diff_time = get_time_diff(start_time, end_time);
			add_entry(result, diff_time, 0);
			RTMB_verbose_printf(stdout, cmd_args, 3,
			        "multi_thread #%d: Difference between end "
				        " and start times = %.3f us\n",
			        result -> result_id, MICROSEC(diff_time));
		}

		if (IS_EXEC_TIME_GRT_THAN_CLK(result, clock_accuracy)) {
			done = TRUE;
		} else {
			done = FALSE;
			sub_iters *= MULTIPLIER_FOR_SUB_ITER;
			free_chain(result, 0);
		}
	}

	return NULL;
}
