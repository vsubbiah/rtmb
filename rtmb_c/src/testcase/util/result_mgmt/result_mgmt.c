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
 
#include <pthread.h>
#include <math.h>
#include <string.h>

#include <RTMB_print.h>
#include <result_mgmt.h>

static int scale = 1000000;
double system_throughput_score;
double system_consistency_score;
double system_score;

/*
 * Name    : add_entry()
 * Desc    : Called after every iteration of a test run to insert the test
 *              result into the result table. This function also computes
 *              the max,min and mean times of a test result chain after
 *               insertion of a new node.
 * Args    :
 *	     result	= test result structure.
 *	     time_diff	= Difference between the end and start time of the test.
 * 	     synch	= A boolean flag to indicate if "synchnonized" add is
 *              needed.
 * Returns : 0 on success and -1 on failure.
 */
int add_entry(test_results_t *result, long long time_diff, int synch) {
	/* Disallow addition until the MAX_TESTS macro is expanded */
	assert(result != (test_results_t *) NULL);

	assert(result -> num_entries <= result -> max_entries - 1);

	if (synch == 1) {
		pthread_mutex_lock(&result->mutex);
	}

	if (result->num_entries == 0) {
		result->mintime = time_diff;
		result->maxtime = time_diff;
	} else {
		if (time_diff > result->maxtime) {
			result->maxtime = time_diff;
		}

		if (time_diff < result -> mintime) {
			result->mintime = time_diff;
		}
	}

	result -> values[result -> num_entries] = time_diff;

	/* Increment the num_entries now */
	result->num_entries++;

	if (result -> num_entries == result -> max_entries) {
		compute_std_time(result, 0);
		result -> mediantime = (long long) get_median(result -> values,
		        result -> num_entries);
	}

	if (synch == 1) {
		pthread_mutex_unlock(&result->mutex);
	}

	/* All done, return 0 */
	return SUCCESS;
}

/*
 * Name    : compute_std_time()
 * Desc    : Computes the std deviation time for a given test_result chain.
 * Args    : 	results = Pointer to test_results chain.
 *		synch	= Bool flag that makes the method "synchronized" when
 *		synch	= true.
 * Returns : void
 */
void compute_std_time(test_results_t *result, int synch) {
	long long diff, sq_diff, total_sq_diff = 0;
	double mean_total_sq_diff = 0;
	long long sum = 0;
	int i = 0;

	if (synch == 1) {
		pthread_mutex_lock(&result->mutex);
	}

	/* Make sure that the pointer received is not NULL */
	assert(result != NULL);

	for (i = 0; i < result -> num_entries; i++) {
		sum += result -> values[i];
	}

#ifdef TRACE_MSG
	RTMB_printf(stderr, " sum  = %lld \n" , sum );
	RTMB_printf(stderr ," num  = %d \n" , result -> num_entries);
#endif

	result -> meantime = sum / result -> num_entries;

	for (i = 0; i < result -> num_entries; i++) {
		diff = result -> values[i] - result -> meantime;
		sq_diff = diff * diff;
		total_sq_diff += sq_diff;
	}

	mean_total_sq_diff = (total_sq_diff / result->num_entries);
	result->stdtime = sqrt(mean_total_sq_diff);
	if (synch == 1) {
		pthread_mutex_unlock(&result->mutex);
	}
}

/*
 * Name   : check_pass_criteria()
 * Desc   : Checks if the completion times for each test run is within
 *	    the acceptable limits.
 *
 *	    Acceptable limits:
 *	    ------------------
 *	    Y% of the outliers are within X% of the average and B%
 *	    the outliers are within A% of the average.
 *	      Ex. 90% of outliers are within 150% of mean and 100%
 *		  within 200% of mean.
 *
 *	    Algorithm:
 *	    ----------
 *	    1. Check if a particular entry is within the computed deviation
 *	       time or not. If not, it is an outlier.
 *	    2. If the entry is an outlier, then check how far away it is
 *	       from the expected mean time mentioned in the input file for
 *	       this test. Record the value and place it in the right bucket:
 *		      a. >X%, but <A% of average.
 *		      b. >B% of average.
 *	    3. Now, check the percentage of outliers in (a) and (b) above.
 *	    4. Return 0 if Y% of the outliers are within X% of the avg
 *	       and (&&) B% of them are within A% of the avg. Else,
 *	       return -1.
 * Args:
 *	results	 = test_results_t - indicating the test result chain.
 *	cmd_args = Pointer to the cmd line args.
 *	bmtest	 = Input parameters for this test to make checks against.
 *	synch	 = Bool flag that makes the method "synchronized" when true.
 * Returns: 0 on success and -1 or error.
 */
int check_pass_criteria(test_results_t *result, cmd_line_args *cmd_args,
        benchmark_test *bmtest, int synch) {

	double positive_limit = 0;
	double negative_limit = 0;
	int test_pass;

	long non_conforming_iterations = 0;
	long min_acceptable_iterations = 0;
	int i;

	double acceptable_percentage = 0;
	double range_percentage = 0;

	if (synch == 1) {
		pthread_mutex_lock(&result->mutex);
	}

	if (cmd_args ->_rt_mode == SOFT_RT) {
		acceptable_percentage
		        = bmtest -> _inp_params -> srt.acceptable_percentage;
		range_percentage
		        = bmtest -> _inp_params -> srt.range_percentage;
	} else if (cmd_args ->_rt_mode == HARD_RT) {
		acceptable_percentage
		        = bmtest -> _inp_params -> hrt.acceptable_percentage;
		range_percentage
		        = bmtest -> _inp_params -> hrt.range_percentage;
	} else if (cmd_args ->_rt_mode == GUARANTEED_RT) {
		acceptable_percentage
		        = bmtest -> _inp_params -> grt.acceptable_percentage;
		range_percentage
		        = bmtest -> _inp_params -> grt.range_percentage;
	}

	min_acceptable_iterations = (result -> num_entries / 100.0)
	        * acceptable_percentage;

	positive_limit = result -> mediantime / 100.0
	        * (100 + range_percentage);
	negative_limit = result -> mediantime / 100.0
	        * (100 - range_percentage);

	for (i = 0; i < result -> num_entries; i++) {
		if (result -> values[i] > positive_limit || result -> values[i]
		        < negative_limit) {
			non_conforming_iterations++;
		}
	}

	if ((result -> num_entries - non_conforming_iterations)
	        >= min_acceptable_iterations) {
		test_pass = SUCCESS;
	} else {
		test_pass = ERROR;
	}

	RTMB_verbose_printf(stdout, cmd_args, 1,
	        "\nMeasured median for this test run: %.3f us\n", MICROSEC(
	                (double) result -> mediantime));
	RTMB_verbose_printf(stdout, cmd_args, 1,
	        "Measured maximum time for this test run: %.3f us\n", MICROSEC(
	                (double) result->maxtime));
	RTMB_verbose_printf(stdout, cmd_args, 1,
	        "Measured minimum time for this test run: %.3f us\n", MICROSEC(
	                (double) result->mintime));
	RTMB_verbose_printf(stdout, cmd_args, 1,
	        "Measured mean for this test run: %.3f us\n", MICROSEC(
	                (double) result->meantime));
	RTMB_verbose_printf(
	        stdout,
	        cmd_args,
	        1,
	        "Measured standard deviation for this test run: %.3f us\n",
	        MICROSEC((double) result -> stdtime));

	RTMB_verbose_printf(stdout, cmd_args, 1,
	        "Expected number of iterations between %.3f us "
		        "and %.3f us: %d \n", MICROSEC(negative_limit),
	        MICROSEC(positive_limit), min_acceptable_iterations);

	RTMB_verbose_printf(stdout, cmd_args, 1,
	        "Total number of conforming iterations: %d \n",
	        result -> num_entries - non_conforming_iterations);

	RTMB_verbose_printf(stdout, cmd_args, 1,
	        "Total number of outlying iterations: %d \n\n",
	        non_conforming_iterations);

	result -> non_conforming_iterations = non_conforming_iterations;
	result -> positive_limit = positive_limit;
	result -> negative_limit = negative_limit;
	result -> min_acceptable_iterations = min_acceptable_iterations;

	if (synch == 1) {
		pthread_mutex_lock(&result->mutex);
	}

	return test_pass;
}

/*
 * Name    : free_chain()
 * Desc    : "free()s" all entries in the hash chain corresponding to a bucket.
 * Args    : result = Pointer to a list of test_results_t
 *	     synch  = Bool flag that makes the method "synchronized" when true.
 * Returns : void
 */
void free_chain(test_results_t *result, int synch) {
	if (synch == 1) {
		pthread_mutex_lock(&result->mutex);
	}

	result -> num_entries = 0;

	if (synch == 1) {
		pthread_mutex_unlock(&result->mutex);
	}
}

/*
 * Name    : free_chain()
 * Desc    : "free()s" all entries in the hash chain corresponding to a bucket.
 * Args    : result = Pointer to a list of test_results_t
 *	     synch  = Bool flag that makes the method "synchronized" when true.
 * Returns : void
 */
void free_chains(test_results_t *result, int len, int synch) {
	int i = 0;

	if (synch == 1) {
		pthread_mutex_lock(&result->mutex);
	}

	for (i = 0; i < len; i++) {
		free_chain(&result[i], 0);
	}

	if (synch == 1) {
		pthread_mutex_unlock(&result->mutex);
	}
}

void add_result_to_result_table1(result_table_t *res_table,
        test_results_t *result, test_bucket_number_t bucket,
        benchmark_test *bmtest) {

	final_bm_test_result_t *fbmt = NULL;
	vector_t *score_vec = create_vector();
	vector_t *result_vec = create_vector();

	test_score_t* tScore = calloc(1, sizeof(test_score_t));

	if (tScore != NULL) {
		add_element(result_vec, (void *) result);
		tScore -> consistency_score = 1;
		tScore -> min_throughput = 1;
		tScore -> max_throughput = 1;
		tScore -> mean_throughput = 1;
		tScore -> wgt_throughput = 1;
		tScore -> throughput_score = 1;
		tScore -> result = result_vec;
		add_element(score_vec, (void *) tScore);
	} else {
		RTMB_printf(stderr, "add_result_to_result_table1:"
			" calloc() failed\n");
		abort();
	}

	fbmt = (final_bm_test_result_t *) calloc(1,
	        sizeof(final_bm_test_result_t));

	if (fbmt != NULL) {
		fbmt -> throughput_test_score = -1;
		fbmt -> consistency_test_score = -1;
		fbmt -> score = score_vec;
	} else {
		RTMB_printf(stderr, "add_result_to_result_table1:"
			" calloc() failed\n");

		abort();
	}

	res_table[bucket].fbmt = fbmt;
	res_table[bucket].bm_test = bmtest;
	res_table[bucket].valid = 1;
}

void add_result_to_result_table2(result_table_t *res_table,
        test_results_t *result, test_bucket_number_t bucket,
        benchmark_test *bmtest) {

	final_bm_test_result_t *fbmt = NULL;
	vector_t* result_vector = create_vector2(result);
	vector_t* score_vector = NULL;

	score_vector = generate_combined_score(result_vector,
	        bmtest ->_reference_score);

	fbmt = create_final_bm_test_result1(score_vector);

	/*
	 * no test/pass distinction
	 * all tests are meant to marked as passed.
	 */
	fbmt -> test_pass = TRUE;
	res_table[bucket].fbmt = fbmt;
	res_table[bucket].bm_test = bmtest;
	res_table[bucket].valid = 1;
}

void add_result_to_result_table3(result_table_t *res_table,
        test_results_t *result, int result_count, test_bucket_number_t bucket,
        benchmark_test *bmtest) {
	int i = 0;
	final_bm_test_result_t *fbmt = NULL;
	vector_t* result_vector = create_vector();
	vector_t *score_vector = NULL;

	for (i = 0; i < result_count; i++) {
		add_element(result_vector, &result[i]);
	}

	score_vector = generate_combined_score(result_vector,
	        bmtest ->_reference_score);
	fbmt = create_final_bm_test_result1(score_vector);

	res_table[bucket].fbmt = fbmt;
	res_table[bucket].bm_test = bmtest;
	res_table[bucket].valid = 1;
}

void add_result_to_result_table4(result_table_t *res_table,
        vector_t *score_vector, test_bucket_number_t bucket,
        benchmark_test *bmtest, char test_pass) {

	final_bm_test_result_t* fbmt = create_final_bm_test_result1(
	        score_vector);

	fbmt -> test_pass = test_pass;
	res_table[bucket].fbmt = fbmt;
	res_table[bucket].bm_test = bmtest;
	res_table[bucket].valid = 1;
}

void add_result_to_result_table5(result_table_t *res_table,
        vector_t *score_vector, test_bucket_number_t bucket,
        benchmark_test *bmtest, char test_pass) {

	final_bm_test_result_t* fbmt = calloc(sizeof(final_bm_test_result_t),
		1);

	fbmt ->consistency_test_score =0;
	fbmt ->throughput_test_score =0;
	fbmt -> test_pass = test_pass;
	fbmt->score = score_vector;

	res_table[bucket].fbmt = fbmt;
	res_table[bucket].bm_test = bmtest;
	res_table[bucket].valid = 1;
}


void add_final_test_result(result_table_t *res_table,
        final_bm_test_result_t *fbmt, test_bucket_number_t bucket) {

	if (res_table != NULL) {
		res_table[bucket].fbmt = fbmt;
	} else {
		RTMB_printf(stderr,
		        "ERROR: free()d/corrupted result table or ");
		RTMB_printf(stderr,
		        "test results structure in add_test_result().");
		RTMB_printf(stderr, " Aborting...\n");
		abort();
	}
}

/*
 * Name    : free_all_entries()
 * Desc    : Calls "free()" for all entries in the result table.
 * Args    : table = pointer to the result_table.
 *	     synch = Bool flag that makes the method "synchronized" when true.
 * Returns : void
 */
void free_all_entries(result_table_t *table, int synch) {
	int i;
	if (synch == 1) {
		/* Become owner of this table */
		pthread_mutex_lock(&table->res_table_mutex);
	}

	if (table != NULL) {
		/* Walk the buckets and delete all entries within each bucket*/
		for (i = 0; i < MAX_TESTS; i++) {
			/* free entries */

		}
	} else {
		RTMB_printf(stderr,
		        "PANIC: Result table looks corrupt! Aborting...\n");
		abort();
	}
	if (synch == 1) {
		/* Exit the lock ownership */
		pthread_mutex_unlock(&table->res_table_mutex);
	}
}

void set_test_result_name(test_results_t *result, char *name) {
	strcpy(result -> name, name);
}

void set_number_of_entries(test_results_t* result, int max_entries) {
	result -> max_entries = max_entries;

	if (result -> values != NULL) {
		free(result -> values);
	}

	result -> values = calloc(max_entries, sizeof(long long));

	if (!result -> values)
		abort();
}

long long get_ideal_sleep_time(result_table_t* res_table) {
	final_bm_test_result_t *fbmt = res_table[SYSTIMER_CONFIG].fbmt;
	vector_t *v = fbmt -> score;
	test_score_t *score = get_element_at(v, 0);
	test_results_t *result = get_element_at(score -> result, 0);

	long long sleep_quantum_time = (long long) result ->opern_amount;

	return sleep_quantum_time;
}

test_results_t* create_test_result(int count, int iters) {
	test_results_t* result = calloc(count, sizeof(test_results_t));
	int i = 0;

	if (result != NULL) {
		for (i = 0; i < count; i++) {
			result[i].iterations = iters;
			result[i].num_entries = 0;
			result[i].max_entries = iters;
			result[i].values = calloc(iters, sizeof(long long));

			if (result[i].values == NULL)
				abort();
		}
		pthread_mutex_init(&result ->mutex, NULL);
	} else {
		RTMB_printf(stderr, "create_test_result:"
			" calloc() failed\n");
		abort();
	}

	return result;
}

test_results_t* create_test_result1(int count, int iters, int num_entries) {
	test_results_t* result = calloc(count, sizeof(test_results_t));
	int i = 0;

	if (result != NULL) {
		for (i = 0; i < count; i++) {
			result[i].iterations = iters;
			result[i].num_entries = 0;
			result[i].max_entries = num_entries;
			result[i].values = calloc(num_entries,
			        sizeof(long long));
		}
		pthread_mutex_init(&result ->mutex, NULL);
	} else {
		RTMB_printf(stderr, "create_test_result1:"
			" calloc() failed\n");
		abort();
	}

	return result;
}

/* deletes all allocated memory for test_result_t structure */
void delete_test_result(test_results_t* result) {
	if (result != NULL) {
		if (result -> values != NULL) {
			free(result->values);
		}
		free(result);
	} else {
		RTMB_printf(stderr, "delete_test_result: Invalid pointer to "
			"test_result_t\n");
		abort();
	}
}

/* returns the number of time the values are normalized */
int normalize_result(test_results_t* result, long long min_time,
        long long normalize_time) {
	int n = result ->num_entries;
	int i = 0;
	int count = 0;

#ifdef TRACE_MSG
	RTMB_printf(stderr, " mintime = %lld \n", result -> mintime);
	RTMB_printf(stderr, " maxtime = %lld num = %d  \n",
		result -> maxtime, n);
	RTMB_printf(stderr, " meantime = %lld \n", result -> meantime);
#endif

	long long *values = result->values;
	for (i = 0; i < n; i++) {
		if (values[i] < min_time) {
			values[i] = normalize_time;
			count++;
		}
	}

	if (result -> mintime < min_time) {
		result -> mintime = normalize_time;
	}

	if (result -> maxtime < min_time) {
		result -> maxtime = normalize_time;
	}

	result -> mediantime = get_median(result -> values,
	        result ->num_entries);
	compute_std_time(result, 0);

#ifdef TRACE_MSG
	RTMB_printf(stderr, " mintime = %lld \n", result -> mintime);
	RTMB_printf(stderr, " maxtime = %lld \n", result -> maxtime);
	RTMB_printf(stderr, " meantime = %lld \n", result -> meantime);
#endif

	return count;
}

test_score_t * create_test_score3(test_results_t* result) {
	test_score_t* tScore = calloc(1, sizeof(test_score_t));

	if (tScore != NULL) {
		vector_t *vec = create_vector2((void *) (result));
		tScore -> consistency_score = 0;
		tScore -> min_throughput = 0;
		tScore -> max_throughput = 0;
		tScore -> mean_throughput = 0;
		tScore -> wgt_throughput = 0;
		tScore -> throughput_score = 0;
		tScore -> result = vec;
	} else {
		RTMB_printf(stderr, "create_test_score3:"
			" calloc() failed\n");
		abort();
	}
	return tScore;
}

test_score_t * create_test_score1(vector_t* result) {
	test_score_t* tScore = calloc(1, sizeof(test_score_t));

	if (tScore != NULL) {
		tScore -> consistency_score = 0;
		tScore -> min_throughput = 0;
		tScore -> max_throughput = 0;
		tScore -> mean_throughput = 0;
		tScore -> wgt_throughput = 0;
		tScore -> throughput_score = 0;
		tScore -> result = result;
	} else {
		RTMB_printf(stderr, "create_test_score1:"
			" calloc() failed\n");
		abort();
	}

	return tScore;
}

test_score_t * create_test_score2() {
	return create_test_score1((vector_t *) NULL);
}

void compute_bm_test_result_scores(final_bm_test_result_t* fbmt) {
	int i = 0;

	vector_t *score_vec = fbmt ->score;
	double throughput_product = 1;
	double consistency_product = 1;

	int number_of_subtests = score_vec -> size;

	if (number_of_subtests == 1) {
		test_score_t* score = get_element_at(score_vec, 0);
		fbmt ->consistency_test_score = score ->consistency_score;
		fbmt ->throughput_test_score = score ->throughput_score;
	} else {
		for (i = 0; i < score_vec -> size; i++) {
			test_score_t* score = get_element_at(score_vec, i);

			throughput_product *= pow(score -> throughput_score,
			        1.0 / number_of_subtests);
			consistency_product *= pow(score -> consistency_score,
			        1.0 / number_of_subtests);
		}

		fbmt ->consistency_test_score = consistency_product;
		fbmt ->throughput_test_score = throughput_product;
	}

	fbmt -> test_score = pow(fbmt -> consistency_test_score
	        * fbmt -> throughput_test_score, 0.5);
}

final_bm_test_result_t *create_final_bm_test_result1(vector_t *score) {
	final_bm_test_result_t* fbmt = (final_bm_test_result_t *) calloc(1,
	        sizeof(final_bm_test_result_t));

	if (fbmt != NULL) {
		fbmt -> throughput_test_score = -1;
		fbmt -> consistency_test_score = -1;
		fbmt -> score = score;
		compute_bm_test_result_scores(fbmt);
	} else {
		RTMB_printf(stderr, "create_final_bm_test_result1:"
			" calloc() failed\n");
		abort();
	}

	return fbmt;
}

final_bm_test_result_t *create_final_bm_test_result2(test_results_t *result) {
	final_bm_test_result_t *fbmt = NULL;
	test_score_t * score = create_test_score3(result);
	vector_t *tscore_vec = create_vector2((void *) score);

	if (tscore_vec != NULL) {
		fbmt = create_final_bm_test_result1(tscore_vec);
	} else {
		RTMB_printf(stderr, "create_final_bm_test_result2:"
			" calloc() failed\n");
		abort();
	}

	return fbmt;
}

double get_min_exec_time(result_table_t* res_table) {
	final_bm_test_result_t *fbmt = res_table[SYSCLOCK_CONFIG].fbmt;
	vector_t *v = fbmt -> score;
	test_score_t *score = get_element_at(v, 0);

	assert(score != NULL);
	test_results_t *result = get_element_at(score -> result, 0);

	assert( result != NULL);

	return (20 * result -> mediantime);
}

void get_ideal_work_time(result_table_t* res_table, long long *min,
        long long *max, long long *median) {
	final_bm_test_result_t *fbmt = res_table[SYSCLOCK_CONFIG].fbmt;
	vector_t *v = fbmt -> score;
	test_score_t *score = get_element_at(v, 0);
	test_results_t *result = get_element_at(score -> result, 0);

	if (median != NULL) {
		*median = 20 * result -> mediantime;
	}

	if (min != NULL) {
		*min = result -> mintime;
	}

	if (max != NULL) {
		*max = result -> maxtime;
	}
}

long long get_clock_accuracy(result_table_t * res1_table) {
	final_bm_test_result_t *fbmt = res1_table[SYSCLOCK_CONFIG].fbmt;
	vector_t *v = fbmt -> score;
	test_score_t *score = get_element_at(v, 0);
	test_results_t *result = get_element_at(score -> result, 0);
	return result -> meantime;
}

long get_ideal_work_quantum(result_table_t* res_table) {
	final_bm_test_result_t *fbmt = res_table[SYSCLOCK_CONFIG].fbmt;
	vector_t *v = fbmt -> score;
	test_score_t *score = get_element_at(v, 0);
	test_results_t *result = get_element_at(score->result, 0);
	return result ->opern_amount;
}

double get_contribution(long long value, double median) {
	double percent = 0;
	double diff = value - median;

	if (diff == 0) {
		percent = 100;
	} else if (diff == -median) {
		percent = 10;
	} else if (diff == median) {
		percent = 0;
	} else {
		/* take absolute value */
		if (diff < 0)
			diff = (-diff);

		if (diff > median) {
			percent = 0;
		} else {
			percent = value * 100.0 / median;

			if (percent > 200.0)
				percent = 0.0;
			else if (percent == 100) {
				percent = 100;
			} else if (percent < 100) {
				percent = (int) (((int) (percent + 10)) / 10);
				percent *= 10;
			} else {
				double difference = percent - 100;
				difference
				        = (int) (((int) (difference)) / 10.0);
				difference = (int) (difference * 10.0);
				percent = 100 - difference;
			}
		}
	}

	return percent;
}

vector_t *generate_combined_score(vector_t *result_vec, double reference_score) {
	double median = 0;
	int value_length = 1;
	int result_length = 1;
	double contribution_percent = 0;
	double combined_throughput = 0;
	double min_throughput = 0;
	double max_throughput = 0;
	double sum_throughput = 0;
	double consistency_sum = 0;
	double throughput = 0, temp = 0;
	long long *values;
	test_results_t* result = NULL;
	long long number_of_measured_times = 0;
	test_score_t *tscore = create_test_score1(result_vec);
	vector_t* score_vec = create_vector2((void *) tscore);
	double *median_array;

	int i = 0, j = 0, k = 0;
	int number_of_results;

	number_of_results = result_vec -> size;

	median_array = calloc(sizeof(double), number_of_results);

	if (median_array == NULL) {
		RTMB_printf(stderr, "unable to allocate memory\n");
		abort();
	}

	for (k = 0; k < number_of_results; k++) {
		result = get_element_at(result_vec, k);
		median_array[k] = result -> mediantime;
	}

	/* sort the median array */
	for (i = 0; i < number_of_results - 1; i++) {
		for (j = i + 1; j < number_of_results; j++) {
			if (median_array[i] > median_array[j]) {
				temp = median_array[i];
				median_array[i] = median_array[j];
				median_array[j] = temp;
			}
		}
	}

	/* find the median of medians */
	if (number_of_results % 2 == 0) {
		int pos1 = number_of_results / 2;
		int pos2 = number_of_results / 2 - 1;
		median = (median_array[pos1] + median_array[pos2]) / 2;
	} else {
		median = (median_array[number_of_results / 2]);
	}

	values = result -> values;
	max_throughput = scale * 1.0 / values[0];
	min_throughput = scale * 1.0 / values[0];

	for (i = 0; i < result_length; i++) {
		result = get_element_at(result_vec, i);
		values = result -> values;
		value_length = result -> num_entries;

		for (j = 0; j < value_length; j++) {
			/* Throughput calculation is here */
			throughput = 1.0 * scale / values[j];

			if (throughput < min_throughput) {
				min_throughput = throughput;
			} else if (throughput > max_throughput) {
				max_throughput = throughput;
			}

			sum_throughput += throughput;

			contribution_percent = get_contribution(values[j],
			        median);
			combined_throughput += (contribution_percent * scale)
			        / (values[j]);

			/* consistency calculation is here */
			consistency_sum += contribution_percent;
		}
	}

	number_of_measured_times = (result_length * value_length);
	tscore ->consistency_score = consistency_sum / number_of_measured_times;
	tscore ->min_throughput = min_throughput;
	tscore ->max_throughput = max_throughput;
	tscore ->mean_throughput = sum_throughput / number_of_measured_times;
	tscore ->wgt_throughput = combined_throughput
	        / (number_of_measured_times * 100.0);

	assert(reference_score > 0);

	tscore -> throughput_score = reference_score / median;

	free(median_array);
	return score_vec;
}

void compute_system_scores(result_table_t* res_table) {
	int number_of_executed_tests = 0;
	benchmark_test* bmtest;
	int i = 0;

	final_bm_test_result_t* fbmt;

	double consistency_product = 1;
	double throughput_product = 1;

	double consistency_test_score;
	double throughput_test_score;

	for (i = 0; i < MAX_TESTS; i++) {
		if (res_table[i].valid) {
			bmtest = res_table[i].bm_test;
			number_of_executed_tests++;
		}
	}

	for (i = 0; i < MAX_TESTS; i++) {
		if (res_table[i].valid) {
			bmtest = res_table[i].bm_test;
			fbmt = res_table[i].fbmt;

			/*
			 * We are trying to compute geometric mean.
			 * Do not multiply all numbers and then
			 * find the nth root since the product can
			 * exceed max value of double.
			 *
			 * Instead mutiply all nth roots.			 *
			 */
			if (fbmt->test_pass) {
				consistency_test_score
				        = fbmt ->consistency_test_score;
				throughput_test_score
				        = fbmt ->throughput_test_score;

				if (consistency_test_score > 0) {
					consistency_product *= pow(
					        consistency_test_score,
					        1.0 / number_of_executed_tests);
				}

				if (throughput_test_score > 0) {
					throughput_product *= pow(
					        throughput_test_score,
					        1.0 / number_of_executed_tests);
				}
			}
		}
	}

	system_consistency_score = consistency_product;
	system_throughput_score = throughput_product;
	system_score = pow(consistency_product * throughput_product, 0.5);
}

int compare_numbers(const void *a, const void *b) {
	long long n1 = (*(long long *) a);
	long long n2 = (*(long long *) b);

	return (n1 > n2);
}

double get_median(long long *values, int n) {
	int i;
	double median_value;
	long long *temp = (long long*) calloc(sizeof(long long), n);

	for (i = 0; i < n; i++) {
		temp[i] = values[i];
	}

	qsort(temp, n, sizeof(long long), compare_numbers);

	if (n % 2 == 1) { /* odd number of elements */
		median_value = temp[n / 2];
	} else {
		median_value = (temp[n / 2 - 1] + temp[n / 2]) / 2.0;
	}

	free(temp);
	return median_value;
}

void clone_results(test_results_t* dest, test_results_t* source) {
	int i;
	long long *values;

	values = dest -> values;

	/* Shallow copy -- do a member by member copy */
	*dest = *source;

	/*
	 * we do not want the pointer "values" to be overwritten
	 * from the pointer from source.
	 */
	dest -> values = values;

	/* Do a deep copy of the values arrays */
	for (i = 0; i < source -> num_entries; i++)
		dest -> values[i] = source -> values[i];
}

double get_acceptable_percent(benchmark_test* test, cmd_line_args* cmd_args) {
	double percent = 0;
	if (cmd_args->_rt_mode == SOFT_RT) {
		percent = test->_inp_params->srt.acceptable_percentage;
	} else if (cmd_args->_rt_mode == HARD_RT) {
		percent = test->_inp_params->hrt.acceptable_percentage;
	} else {
		percent = test->_inp_params->grt.acceptable_percentage;
	}

	return percent;
}

void set_acceptable_percent(benchmark_test* test, cmd_line_args* cmd_args,
        double percent) {
	if (cmd_args->_rt_mode == SOFT_RT) {
		test->_inp_params->srt.acceptable_percentage = percent;
	} else if (cmd_args->_rt_mode == HARD_RT) {
		test->_inp_params->hrt.acceptable_percentage = percent;
	} else {
		test->_inp_params->grt.acceptable_percentage = percent;
	}
}

double get_range_percent(benchmark_test* test, cmd_line_args* cmd_args) {
	double percent = 0;
	if (cmd_args->_rt_mode == SOFT_RT) {
		percent = test->_inp_params->srt.range_percentage;
	} else if (cmd_args->_rt_mode == HARD_RT) {
		percent = test->_inp_params->hrt.range_percentage;
	} else {
		percent = test->_inp_params->grt.range_percentage;
	}

	return percent;
}

void set_range_percent(benchmark_test* test, cmd_line_args* cmd_args,
        double percent) {
	if (cmd_args->_rt_mode == SOFT_RT) {
		test->_inp_params->srt.range_percentage = percent;
	} else if (cmd_args->_rt_mode == HARD_RT) {
		test->_inp_params->hrt.range_percentage = percent;
	} else {
		test->_inp_params->grt.range_percentage = percent;
	}
}

void fill_acceptable_percentages(benchmark_test* test, cmd_line_args* cmd_args,
        double percentage, double range, double *old_percentage,
        double *old_range) {

	if (cmd_args->_rt_mode == HARD_RT) {
		if (old_percentage != NULL) {
			*old_percentage
			        = test->_inp_params->hrt.acceptable_percentage;
		}

		if (old_range != NULL) {
			*old_range = test->_inp_params->hrt.range_percentage;
		}

		test->_inp_params->hrt.acceptable_percentage = percentage;
		test->_inp_params->hrt.range_percentage = range;
	} else if (cmd_args->_rt_mode == SOFT_RT) {
		if (old_percentage != NULL) {
			*old_percentage
			        = test->_inp_params->srt.acceptable_percentage;
		}

		if (old_range != NULL) {
			*old_range = test->_inp_params->srt.range_percentage;
		}

		test->_inp_params->srt.acceptable_percentage = percentage;
		test->_inp_params->srt.range_percentage = range;
	} else if (cmd_args->_rt_mode == GUARANTEED_RT) {
		if (old_percentage != NULL) {
			*old_percentage
			        = test->_inp_params->grt.acceptable_percentage;
		}

		if (old_range != NULL) {
			*old_range = test->_inp_params->grt.range_percentage;
		}

		test->_inp_params->grt.acceptable_percentage = percentage;
		test->_inp_params->grt.range_percentage = range;
	}
}
