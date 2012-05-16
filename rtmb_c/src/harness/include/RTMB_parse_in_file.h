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
 
#ifndef __IN_FL_PARSER__
#define __IN_FL_PARSER__

#include <stdlib.h>
#include <assert.h>
#include <stdio.h>

#include <RTMB_common.h>
#include <RTMB_rd_cmd_line.h>

/* Max size of a single line in the input file */
#define MAX_INPUT_LINE_SIZE 1024

/* Test name length */
#define TESTNAME_SIZE 32

/* Test desc length */
#define TESTDESC_SIZE 64

/* Global data structures */

typedef struct _test_times {

	/** Start, maximum, minimum, mean and std dev times */

	long long _max_time;
	long long _min_time;
	long long _mean_time;
	float _std_time;
} test_times;

typedef struct _benchmark_test {

	/** Test name */
	char _name[TESTNAME_SIZE];

	/** Test description */
	char _desc[TESTDESC_SIZE];

	/** Time taken by the tests */
	test_times _times;

	/** Number of iterations of each test */
	int _iterations;

	/** Pointer to the next test entry */
	struct _benchmark_test *_next;

	/** Pointer back to the input_params structure */
	struct _input_params *_inp_params;

	/** bool - Is this a configuration test */
	int _config_test;

	/** Number of times to retry for tighter deviation */
	int _threshold;

	/** reference score */
	long double _reference_score;
} benchmark_test;

typedef enum exp_values {
	SRT_ACCEPTABLE_PERCENT,
	SRT_RANGE_PERCENT,
	SRT_MULTIPLIER,
	HRT_ACCEPTABLE_PERCENT,
	HRT_RANGE_PERCENT,
	HRT_MULTIPLIER,
	GRT_ACCEPTABLE_PERCENT,
	GRT_RANGE_PERCENT,
	GRT_MULTIPLIER
} exp_vals_t;

/* hard rt conditions */
typedef struct hrt_vals {
	float acceptable_percentage;
	float range_percentage;
	float multiplier;
} hrt_vals_t;

/* soft rt conditions */
typedef struct srt_vals {
	float acceptable_percentage;
	float range_percentage;
	float multiplier;
} srt_vals_t;

/* soft rt conditions */
typedef struct guaranteed_rt_vals {
	float acceptable_percentage;
	float range_percentage;
	float multiplier;
} guaranteedrt_vals_t;

typedef struct _input_params {
	/** List of tests to be run */
	benchmark_test *_tests;

	hrt_vals_t hrt;
	srt_vals_t srt;
	guaranteedrt_vals_t grt;
} input_params;

typedef enum param {
	NAME, DESC, ITERATIONS, CONFIG_TEST, THRESHOLD, REFERENCE_SCORE
} param_t;

/* Function declarations */

int parse_input_file(char *, input_params *);
int set_input_params(benchmark_test *, char *, param_t);
benchmark_test *alloc_new_benchmark_test();
void inputfile_parser_cleanup(input_params *ifl_params);
void insert_new_test_entry(input_params *, benchmark_test *);
int set_exp_values(input_params *, char *, exp_vals_t);

#endif /* __IN_FL_PARSER__ */
