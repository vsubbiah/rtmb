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
 
#ifndef __RESULT_MGMT__
#define __RESULT_MGMT__

/*
 * This header file contains data structures and functions
 * used for managing the results generated for each of the
 * test runs.
 *
 * We maintain a table for this purpose and each linked list
 * in the table corresponds to one particular test type and
 * each entry in the chain corresponds to an individual test
 * run (there will be "benchmark_test->_iterations" number of
 * iterations in each list - see sources that handle input
 * file parsing more details).
 */

/* All defines and includes are below */

#include <time.h>
#include <math.h>
#include "vector.h"

#include "RTMB_parse_in_file.h"
#include "RTMB_rd_cmd_line.h"

#define SUCCESS 0
#define ERROR -1
#define TEST_DESC_SIZE 1024
#define TEST_NAME_SIZE 100

#define TEST_PASSED 1
#define TEST_FAILED 0

#define ACCEPTABLE_DEVIATIONS 1
#define UNACCEPTABLE_DEVIATIONS 0

#define ASSIGN_MAX(x,y) if((x) < (y)) (x) = (y)
#define ASSIGN_MIN(x,y) if((x) > (y)) (x) = (y)
#define MICROSEC(x)  (((x)/1000.0))
#define MULTIPLIER_FOR_SUB_ITER 2
#define IS_EXEC_TIME_GRT_THAN_CLK(result, clk) (((result)) -> mediantime > (clk))

/* The below enum is used to point to individual test bucket */

typedef enum test_bucket_number {
	SYSCLOCK_CONFIG,
	SYSTIMER_CONFIG,
	IRATE_CONFIG,
	FRATE_CONFIG,
	MCACHE_CONFIG,
	DISKIO_WRITE_CONFIG,
	DISKIO_READ_CONFIG,
	NETIO_CONFIG,
	TIMER_ACCURACY_TEST,
	EVENT_DISPATCH_TEST,
	THREAD_PRIORITY_TEST,
	LK_CONSISTENCY_TEST,
	PERIODIC_EVENTS_TEST,
	ASYNC_EVENTS_TEST,
	DISK_IO_TEST,
	NET_IO_TEST,
	MULTI_THREAD_TEST,
	MULTI_CPU_TEST,
	MAX_TESTS
} test_bucket_number_t;

/* Structure for holding complete times of each iteration */

typedef struct test_entry {
	long long complete_time; /* time taken to complete this iteration */
	struct test_entry *next; /* Points to the next node in the chain */
} test_entry_t;

/* Some tests can have multiple test results. So, each such result is
 * recorded within the following structure
 */
typedef struct test_results {
	int result_id; /* Test ID - for different scores */
	long long meantime; /* Mean time taken for test run */
	long long mediantime; /* median time taken for test run */
	float stdtime; /* Std deviation time taken for test run */
	long long maxtime; /* Maximum time taken for the test run */
	long long mintime; /* Minimum time taken for the test run */
	int num_entries; /* Number of entries in this chain */
	int opern_amount; /* Size of work done for config test success*/
	long long *values; /* Points to the head of the chain */
	struct test_results *next; /* Pointer to the next test result struct */
	pthread_mutex_t mutex; /* Mutex to protect test results structure */
	char desc[TEST_DESC_SIZE]; /* Test Result description */
	int rc; /* Indicates if test score is valid or not */

	int iterations;
	char name[TEST_NAME_SIZE];
	int max_entries;

	int non_conforming_iterations;
	int min_acceptable_iterations;
	double negative_limit;
	double positive_limit;
	int pass;
} test_results_t;

typedef struct final_bm_test_result {
	vector_t *score;
	double throughput_test_score;
	double consistency_test_score;
	double test_score;
	char test_pass; /* 0 for fail and 1 for pass */
} final_bm_test_result_t;

typedef struct result_table {
	pthread_mutex_t res_table_mutex; /* Mutex to protect the result table */
	pthread_mutex_t test_bucket_mutex; /* Mutex to protect a bucket within
						the table */
	int bucket; /* Indicator for a test bucket. TestId */

	vector_t *score;
	final_bm_test_result_t * fbmt;
	benchmark_test* bm_test;
	char valid; /* an entry to say that this result_table is valid or not */

} result_table_t;

typedef struct system_score {
	double consistencyScoreForSystem;
	double throughputScoreForSystem;
} system_score_t;

typedef struct test_score {
	char name[100];
	vector_t *result; /* a vector of tResults */
	double consistency_score;
	double min_throughput;
	double max_throughput;
	double mean_throughput;
	double wgt_throughput;
	double throughput_score;
} test_score_t;

test_results_t* create_test_result(int count, int iters);

final_bm_test_result_t *create_final_bm_test_result1(vector_t *score);

final_bm_test_result_t *create_final_bm_test_result2(test_results_t *result);

test_score_t * create_test_score1(vector_t *tResult);
test_score_t * create_test_score2();
test_score_t * create_test_score3(test_results_t* tResult);

/*
 * Add a new entry corresponding to the result of an iteration of the test run.
 * Compute the mean, min and max times also.
 */

int add_entry(test_results_t *, long long, int);

/*
 * Computes the std dev time taken for the entire test run.
 */

void compute_std_time(test_results_t *, int);

/*
 * Check if the std dev time is as expected in the input file.
 */

int check_pass_criteria(test_results_t *, cmd_line_args *, benchmark_test *,
        int);

/*
 * free()s all results for a particular test.
 */

void free_chain(test_results_t *, int);

/*
 * Add a test result into the result table.
 */
void add_test_result(result_table_t *, test_results_t *, test_bucket_number_t,
        int);

/* free the memory of the result struct */
void delete_test_result(test_results_t* result);
/*
 * Gets the time that should be an ideal sleep time based
 * on config test run
 */
long long get_ideal_sleep_time(result_table_t* res_table);
long get_ideal_work_quantum(result_table_t* res_table);
void compute_system_scores(result_table_t* res_table);

void set_test_result_name(test_results_t *result, char *name);

/* returns a vector of test score */
vector_t *generate_combined_score(vector_t *tresult_vec, double referenceScore);

void add_result_to_result_table1(result_table_t *, test_results_t *,
        test_bucket_number_t, benchmark_test *);

void add_result_to_result_table2(result_table_t *res_table,
        test_results_t *result, test_bucket_number_t, benchmark_test *);

void add_result_to_result_table3(result_table_t *res_table,
        test_results_t *result, int result_count, test_bucket_number_t bucket,
        benchmark_test *bmtest);

double get_contribution(long long value, double mean);
long long get_clock_accuracy();

/* returns the number of time the values are normalized */
int normalize_result(test_results_t* result, long long min_time,
        long long normalize_time);

long long get_clock_accuracy(result_table_t * res_table);
void compute_bm_test_result_scores(final_bm_test_result_t* fbmt);
test_results_t* create_test_result1(int count, int iters, int num_entries);
void add_result_to_result_table4(result_table_t *res_table,
        vector_t *score_vector, test_bucket_number_t bucket,
        benchmark_test *bmtest, char);

void set_number_of_entries(test_results_t* result, int max_entries);

extern double system_throughput_score;
extern double system_consistency_score;
extern double system_score;

void free_chains(test_results_t *result, int len, int synch);
double get_median(long long *values, int n);
void clone_results(test_results_t* dest, test_results_t* src);
int can_retry(int done, int *attempt_number, int max_retry,
        double median_of_test);

void fill_acceptable_percentages(benchmark_test* bmtest,
        cmd_line_args* cmd_args, double percentage, double range,
        double *old_percentage, double *old_range);

double get_acceptable_percent(benchmark_test* test, cmd_line_args* cmd_args);

double get_range_percent(benchmark_test* test, cmd_line_args* cmd_args);

void set_range_percent(benchmark_test* test, cmd_line_args* cmd_args,
        double percent);

void set_acceptable_percent(benchmark_test* test, cmd_line_args* cmd_args,
        double percent);
double get_min_exec_time(result_table_t* res_table);

void add_result_to_result_table5(result_table_t *res_table,
        vector_t *score_vector, test_bucket_number_t bucket,
        benchmark_test *bmtest, char test_pass);
#endif /* __RESULT_MGMT__ */

