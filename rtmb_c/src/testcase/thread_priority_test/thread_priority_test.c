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
 
#include <thread_priority_test.h>

/* Globals */
thread_syncers_t *tsyncers = NULL;
thread_syncers_t *parent_sema = NULL;
thread_id_t *thr_id = NULL;
int counter = 0;
static cmd_line_args* cmd_args; /* stores cmd line args */
static int priority_values[3]; /*  stores high,low and medium priorities*/

#define MATRIX_SIZE 5
#define MATRIX_MAX_VAL 100

void print_result_values(test_results_t *result, int ncpus) {
	int i = 0;
	for (i = 0; i < ncpus; i++) {
		fprintf(stdout, "execution time2 = %.3f \n", MICROSEC(
		        result -> values[i]));
	}
}

/*
 * Name     : thread_priority()
 * Desc     : Tests if higher priority threads preempt lower priority threads.
 * Args     : args - Test arguments passed from harness.
 * Returns  : NULL.
 */
void *thread_priority(void *args) {
	int err_flag = SUCCESS;
	int thread_number = 0;
	/* Get the args passed from parent thread */
	thread_args_t *targs = (thread_args_t *) args;
	cmd_args = targs->cmd_args;
	benchmark_test *bmtest = targs->bmtest;
	result_table_t *res_table = targs->res_table;
	char pass_status = TRUE;

	test_results_t *result_low_priority = NULL;
	test_results_t *result_med_priority = NULL;
	test_results_t *result_high_priority = NULL;

	/* Locals */
	int online_cpus = 0;
	int i = 0;
	thr_args_t *low_prio_args = NULL, *med_prio_args = NULL,
	        *high_prio_args = NULL;
	pthread_attr_t *high_prio_attr = NULL, *low_prio_attr = NULL,
	        *med_prio_attr = NULL;
	int new_iters = bmtest->_iterations;
	int total_thrs = 0;
	struct timespec wait_time;
	test_results_t* consolidated_result;
	long opern_amount = 0;

	RTMB_verbose_printf(stdout, cmd_args, 1,
	        "\nTest report for thread_priority test:\n");
	RTMB_verbose_printf(stdout, cmd_args, 1,
	        "====================================\n");
	RTMB_verbose_printf(stdout, cmd_args, 1,
	        "\nthread_priority: Total number of iterations = %d\n\n",
	        new_iters);

	/* Get the total number of online CPUs on the system */
	online_cpus = get_num_online_cpus();
	if (online_cpus == 1) {
		RTMB_verbose_printf(stdout, cmd_args, 1,
		        "thread_priority: Test not done on uniprocessor"
			        " systems\n");
		targs->ret = SUCCESS;
		return (void *) NULL;
	}
	total_thrs = online_cpus * MAX_PRIO_TYPES;
	thr_id = (thread_id_t *) malloc(sizeof(thread_id_t) * online_cpus
	        * MAX_PRIO_TYPES);
	if (thr_id == NULL) {
		RTMB_printf(stderr,
		        "ERROR: malloc() failed in thread_priority()\n");
		abort();
	} else {
		memset(thr_id, 0, (sizeof(thread_id_t) * online_cpus
		        * MAX_PRIO_TYPES));
	}

	/* Alloc memory for test_results */

	result_high_priority = create_test_result1(1, new_iters, online_cpus);
	result_low_priority = create_test_result1(1, new_iters, online_cpus);
	result_med_priority = create_test_result1(1, new_iters, online_cpus);
	consolidated_result = create_test_result1(1, new_iters, online_cpus);

	if (result_low_priority == NULL || result_med_priority == NULL
	        || result_high_priority == NULL || consolidated_result == NULL) {
		RTMB_printf(stderr,
		        "ERROR: malloc() failed in thread_priority()\n");
		abort();
	}
	for (i = 0; i < MAX_PRIO_TYPES; i++) {
		switch (i) {
		case LOW_PRIORITY:
			result_low_priority->result_id = LOW_PRIORITY;
			break; /* break from switch */
		case MED_PRIORITY:
			result_med_priority->result_id = MED_PRIORITY;
			break; /* break from switch */
		case HIGH_PRIORITY:
			result_high_priority->result_id = HIGH_PRIORITY;
			break; /* break from switch */
		}
	}
	/*
	 * Allocate memory for the mutex and cond vars that we use during
	 * this test
	 */
	tsyncers = (thread_syncers_t *) malloc(sizeof(thread_syncers_t));
	if (tsyncers == NULL) {
		RTMB_printf(stderr,
		        "ERROR: malloc() failed in thread_priority()\n");
		abort();
	}
	memset(tsyncers, 0, sizeof(thread_syncers_t));
	pthread_mutex_init(&tsyncers->mutex, NULL);
	pthread_cond_init(&tsyncers->cond, NULL);

	parent_sema = (thread_syncers_t *) malloc(sizeof(thread_syncers_t));
	if (parent_sema == NULL) {
		RTMB_printf(stderr,
		        "ERROR: malloc() failed in thread_priority()\n");
		abort();
	}
	memset(parent_sema, 0, sizeof(thread_syncers_t));
	pthread_mutex_init(&parent_sema->mutex, NULL);
	pthread_cond_init(&parent_sema->cond, NULL);

	priority_values[LOW_PRIORITY] = LOW_PRIO_VAL;
	priority_values[MED_PRIORITY] = MED_PRIO_VAL;
	priority_values[HIGH_PRIORITY] = HIGH_PRIO_VAL;

	/* Create and start "online_cpus" number of high priority threads */
	high_prio_attr = (pthread_attr_t *) malloc(sizeof(pthread_attr_t));
	high_prio_args = (thr_args_t *) malloc(sizeof(thr_args_t));
	med_prio_attr = (pthread_attr_t *) malloc(sizeof(pthread_attr_t));
	med_prio_args = (thr_args_t *) malloc(sizeof(thr_args_t));
	low_prio_attr = (pthread_attr_t *) malloc(sizeof(pthread_attr_t));
	low_prio_args = (thr_args_t *) malloc(sizeof(thr_args_t));

	if (high_prio_args == NULL || high_prio_attr == NULL || med_prio_args
	        == NULL || med_prio_attr == NULL || low_prio_attr == NULL
	        || low_prio_args == NULL) {
		RTMB_printf(stderr,
		        "ERROR: malloc() failed in thread_priority()\n");
		abort();
	}

	/* Create and start "online_cpus" number of high priority threads */
	err_flag = create_start_threads(high_prio_attr, HIGH_PRIORITY,
	        high_prio_args, result_high_priority, consolidated_result,
	        opern_amount, online_cpus, new_iters);

	if (err_flag == ERROR) {
		RTMB_printf(stderr,
		        "ERROR: Thread creation failed in thread_priority()\n");
		abort();
	}

	/* Create and start "online_cpus" number of medium priority threads */
	err_flag = create_start_threads(med_prio_attr, MED_PRIORITY,
	        med_prio_args, result_med_priority, NULL,
	        opern_amount, online_cpus, new_iters);

	if (err_flag == ERROR) {
		RTMB_printf(stderr,
		        "ERROR: Thread creation failed in thread_priority()\n");
		abort();
	}

	/* Create and start "online_cpus" number of low priority threads */
	err_flag = create_start_threads(low_prio_attr, LOW_PRIORITY,
	        low_prio_args, result_low_priority, NULL,
	        opern_amount, online_cpus, new_iters);

	if (err_flag == ERROR) {
		RTMB_printf(stderr,
		        "ERROR: Thread creation failed in thread_priority()\n");
		abort();
	}

	wait_time.tv_sec = 1;
	wait_time.tv_nsec = 0;
	while (counter != total_thrs) {
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

	if (counter == 0) {
		set_test_result_name(result_high_priority,
		        " high priority thread ");
	}

	for (i = 0; i < total_thrs; i++) {
		pthread_join(thr_id[i].tid, NULL);
	}

	long long high_pri_mintime, high_pri_maxtime = 0;
	long long med_pri_mintime, med_pri_maxtime = 0;
	long long low_pri_mintime, low_pri_maxtime = 0;

	high_pri_mintime = result_high_priority -> mintime;
	high_pri_maxtime = result_high_priority -> maxtime;
	med_pri_mintime = result_med_priority -> mintime;
	med_pri_maxtime = result_med_priority -> maxtime;
	low_pri_mintime = result_low_priority -> mintime;
	low_pri_maxtime = result_low_priority -> maxtime;

	if ((high_pri_maxtime < low_pri_mintime && high_pri_maxtime
	        < med_pri_mintime) || (med_pri_maxtime < low_pri_mintime)) {
		/* functional test passed */

	} else {
		RTMB_printf(stderr, "thread_priority: ERROR."
			" Lower priority thread");
		RTMB_printf(stderr, " completed before the ");
		RTMB_printf(stderr, "higher priority thread\n");
		pass_status = FALSE;
	}

	thread_number = 0;

	for (i = 0; i < online_cpus; i++) {
		RTMB_verbose_printf(stdout, cmd_args, 2,
		        "thread_priority: Stop Time of thread %d = %.3f us \n",
		        thread_number, MICROSEC(
		                result_high_priority -> values[i]));
		thread_number++;
	}

	for (i = 0; i < online_cpus; i++) {
		RTMB_verbose_printf(stdout, cmd_args, 2,
		        "thread_priority: Stop Time of thread %d = %.3f us \n",
		        thread_number, MICROSEC(
		                result_med_priority -> values[i]));
		thread_number++;
	}

	for (i = 0; i < online_cpus; i++) {
		RTMB_verbose_printf(stdout, cmd_args, 2,
		        "thread_priority: Stop Time of thread %d = %.3f us \n",
		        thread_number, MICROSEC(
		                result_low_priority -> values[i]));
		thread_number++;
	}

	for (i = 0; i < online_cpus; i++) {
		RTMB_verbose_printf(stdout, cmd_args, 1,
		        "thread_priority: Difference between end "
			        "and start times = %.3f us \n", MICROSEC(
		                consolidated_result -> values[i]));
	}

	if (pass_status == FALSE) {
		test_score_t *score = create_test_score3(consolidated_result);
		vector_t *score_vec = create_vector2(score);
		add_result_to_result_table5(res_table, score_vec,
		        THREAD_PRIORITY_TEST, bmtest, TEST_FAILED);
	} else {
		add_result_to_result_table2(res_table, consolidated_result,
		        THREAD_PRIORITY_TEST, bmtest);

	}

	cleanup(low_prio_args, med_prio_args, high_prio_args, low_prio_attr,
	        med_prio_attr, high_prio_attr);

	targs ->ret = SUCCESS;
	fflush(stderr);
	fflush(stdout);
	return (void *) NULL;
}

/*
 * Name     : cleanup()
 * Desc     : free()s malloced memory during the test
 * Args     : low_prio_args, med_prio_args and high_prio_args - thread args.
 *	      med_prio_attr, low_prio_attr and high_prio_attr - thread
 *	      	priorities.
 * Returns  : void
 */
void cleanup(thr_args_t *low_prio_args, thr_args_t *med_prio_args,
        thr_args_t *high_prio_args, pthread_attr_t *low_prio_attr,
        pthread_attr_t *med_prio_attr, pthread_attr_t *high_prio_attr) {
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
	if (high_prio_attr != NULL) {
		pthread_attr_destroy(high_prio_attr);
		free(high_prio_attr);
	}
	if (med_prio_attr != NULL) {
		pthread_attr_destroy(med_prio_attr);
		free(med_prio_attr);
	}
	if (low_prio_attr != NULL) {
		pthread_attr_destroy(low_prio_attr);
		free(low_prio_attr);
	}
	if (low_prio_args != NULL) {
		free(low_prio_args);
	}
	if (med_prio_args != NULL) {
		free(med_prio_args);
	}
	if (high_prio_args != NULL) {
		free(high_prio_args);
	}
	if (thr_id != NULL) {
		free(thr_id);
	}
}

/*
 * Name    : create_start_threads()
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
int create_start_threads(pthread_attr_t *attr, int type, thr_args_t *args,
        test_results_t *result, test_results_t* consolidated_results,
        int opern_amount, int online_cpus, int iterations) {
	int i;
	static int count = 0;

	pthread_attr_init(attr);

	args->type = type;
	args->opern_amount = opern_amount;
	args->iterations = iterations;
	args->cmd_args = cmd_args;
	args->attr = attr;
	args->policy = SCHED_FIFO;
	args->online_cpus = online_cpus;
	args->results = result;
	args->consolidated_results = consolidated_results;
	int priority = priority_values[type];

	if (set_pthreadattr_sched_param(args->attr, args->policy, priority)
	        == ERROR) {
		return ERROR;
	}

	for (i = 0; i < args->online_cpus; i++) {
		if (pthread_create(&thr_id[count++].tid, args->attr, _start,
		        (void *) args) == ERROR) {
			perror("create_start_threads");
			RTMB_printf(stderr,
			        "create_start_threads: pthread_create()"
				        " failed\n");
			return ERROR;
		}
	}

	return SUCCESS;
}

/*
 * Name    : lock_inc_counter()
 * Desc    : Increment the thread counter under protection of a lock.
 * Args    : total_thrs = total number of threads of all prios started.
 * Returns : void
 */
void lock_inc_counter(int total_thrs) {
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
void lock_dec_counter() {
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

void matrix_multiply(int a[MATRIX_SIZE][MATRIX_SIZE],
        int b[MATRIX_SIZE][MATRIX_SIZE], int c[MATRIX_SIZE][MATRIX_SIZE]) {
	int i, j, k;
	int sum;
	int matrix_size = MATRIX_SIZE;

	for (i = 0; i < matrix_size; i++) {
		for (j = 0; j < matrix_size; j++) {
			sum = 0;

			for (k = 0; k < matrix_size; k++) {
				sum += a[i][k] * b[k][j];

			}

			c[i][j] = sum;
		}
	}
}

/*
 * Name    : _start()
 * Desc    : Start method for a thread.
 * Args    : args - Thread arguments.
 * Returns : NULL
 */
void *_start(void *args) {
	thr_args_t *targs = (thr_args_t *) args;
	int type = targs->type;
	int new_iters = targs->iterations;

	test_results_t *results = targs->results;
	test_results_t *consolidated_results = targs -> consolidated_results;

	struct timespec end_time;
	struct timespec start_time;

	int total_thrs = targs->online_cpus * MAX_PRIO_TYPES;
	struct sched_param param;
	int policy = -2;

	int matrix_size = MATRIX_SIZE;
	int max_value = MATRIX_MAX_VAL;

	int a[MATRIX_SIZE][MATRIX_SIZE];
	int b[MATRIX_SIZE][MATRIX_SIZE];
	int c[MATRIX_SIZE][MATRIX_SIZE];
	int i = 0, j = 0;

	unsigned int seedp = time(NULL);

	for (i = 0; i < matrix_size; i++) {
		for (j = 0; j < matrix_size; j++)
			a[i][j] = 1 + (int) (max_value * (rand_r(&seedp)
			        / (RAND_MAX + 1.0)));
	}

	for (i = 0; i < matrix_size; i++) {
		for (j = 0; j < matrix_size; j++)
			b[i][j] = 1 + (int) (max_value * (rand_r(&seedp)
			        / (RAND_MAX + 1.0)));

	}

#ifdef TRACE_MSG
	RTMB_printf(stderr , "  inside start  \n");
#endif

	if (get_sched_param(pthread_self(), &policy, &param) != SUCCESS) {
		RTMB_printf(stderr,
		        " unable to get policy and priority and priority of"
			        " thread \n");
		policy = -1;
		param.sched_priority = -1;
		/* Assert in the next line will abort. We have a hard failure */
	}

	assert((param.sched_priority == priority_values[type]) && (policy
	        == SCHED_FIFO));
	assert(counter <= total_thrs);

	/* Get the end time */
	if (get_cur_time(&start_time) == ERROR) {
		results->rc = ERROR;
		return (void *) NULL;
	}

	/* Increment the counter with protection and wait for all to start */
	lock_inc_counter(total_thrs);

#ifdef TRACE_MSG
	RTMB_printf(stderr , "  inside after unlocked  %lld \n" ,
		pthread_self());
#endif

	for (i = 0; i < new_iters; i++)
		matrix_multiply(a, b, c);

	/* Get the end time */
	if (get_cur_time(&end_time) == ERROR) {
		lock_dec_counter();
		results->rc = ERROR;
		return (void *) NULL;
	}

	fflush(stderr);
	fflush(stdout);

	/* Add this into the test results*/
	add_entry(results, convert_time_to_nsec(end_time), 1);

	if (consolidated_results) {
		long long difference = ((convert_time_to_nsec(end_time)
		        - convert_time_to_nsec(start_time))) / new_iters;

		add_entry(consolidated_results, difference, 1);
	}

#ifdef TRACE_MSG
	RTMB_printf(stderr , "  before decrement %lld \n" ,
		pthread_self());
#endif

	/* Decrement the counter and return */
	lock_dec_counter();

	results->rc = SUCCESS;
	return (void *) NULL;
}
