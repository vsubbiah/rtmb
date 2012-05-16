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
 
#include <sys/time.h>
#include <sys/types.h>
#include <sys/stat.h>
#include "reporter.h"
#include <errno.h>

char* get_output_dir(char *report_dir) {
	time_t t = time(NULL);
	struct tm *tim = localtime(&t);
	struct timeval tval;

	gettimeofday(&tval, NULL);

	static char buf[1024];
	char format[] = "c_report_YYMMDD_HHMMSSss";

	const char* dir = report_dir;
	int status = 0;

	if (strlen(dir) + sizeof(format) + 1 > sizeof(buf) - 1) {
		// buffer overflow
		return NULL;
	}

	/* Dont worry abt y3k problem for now ( note year is in two digit */
	sprintf(buf, "%s/c_report_%02d%02d%02d_%02d%02d%02d%02d", dir,
	        (tim->tm_year + 1900) % 100, tim -> tm_mon + 1, tim->tm_mday,
	        tim->tm_hour, tim->tm_min, tim->tm_sec, ((int) tval.tv_usec
	                / 1000));

	status = mkdir(buf, 0744);

	if (status != 0) {
		int err = errno;
		char* err_str = strerror(err);
		RTMB_printf(stderr, "ERROR in output drectory creation , "
			"msg = %s \n", err_str);
		return NULL;
	}
	return buf;
}

void dump_result_table_entry(char *report_dir, result_table_t* result_record,
        int verbose_level) {
	char fullpath[1024];
	char *test_report;
	FILE *fp;

	int i = 0;

	vector_t* score_vec;
	final_bm_test_result_t* fbmt;
	benchmark_test* bmtest;

	char* output_dir = get_output_dir(report_dir);

	if (output_dir == NULL) {
		/* cannot write report since directory is not created  */
		return;
	}

	for (i = 0; i < MAX_TESTS; i++) {
		/*
		 * Some specific tests may have been run. So, skip the ones
		 * that were not asked to run. This is indicated by a null
		 * value in result_table for that test.
		 */
		if (result_record[i].bm_test == NULL) {
			continue;
		}
		bmtest = result_record[i].bm_test;
		sprintf(fullpath, "%s/%s", output_dir, bmtest ->_name);
		if (mkdir(fullpath, 0744) != 0) {
			int err = errno;
			RTMB_printf(stderr, "ERROR in report "
				"directory creation , msg = %s \n", strerror(
			        err));
			return;
		}

		strcat(fullpath, "/report.txt");
		test_report = fullpath;
		fp = fopen(test_report, "w");
		if (fp == NULL) {
			RTMB_printf(stderr,
			        "Unable to create output file :%s ",
			        test_report);
			return;
		}
		fbmt = result_record[i].fbmt;
		score_vec = fbmt -> score;
		fprintf(fp, "Report for test: %s\n",
		        result_record[i].bm_test->_name);
		fprintf(fp, "===================================\n\n");

		fprintf(fp, "Score for determinism: %.3f\n",
		        fbmt ->consistency_test_score);
		fprintf(fp, "Score for throughput:  %.3f\n",
		        fbmt ->throughput_test_score);

		dump_score(score_vec, fp, bmtest, verbose_level);
		fclose(fp);

		/* write system report */
		sprintf(test_report, "%s/%s", output_dir, SYSTEM_REPORT_FILE);
		fp = fopen(test_report, "w");
		if (fp == NULL) {
			RTMB_printf(stderr,
			        "Unable to create output file :%s ",
			        test_report);
			return;
		}

		fprintf(fp, "Final throughput score for the system : %.3f\n",
		        system_throughput_score);
		fprintf(fp, "Final consistency score for the system:  %.3f\n",
		        system_consistency_score);
		fprintf(fp, "\nFinal score for the system : %.3f\n",
		        system_score);
	}
}

void print_result(FILE *fp, test_results_t* result, int verbose_level) {

	fprintf(fp, "Total number of iterations of test run:  %d \n",
	        result -> iterations);

	fprintf(fp, "Measured Median for this test run: %.3f us \n",
		MICROSEC((double) result -> mediantime ));

	fprintf(fp,
	        "Measured maximum time for this test run: %.3f us\n",
	        MICROSEC((double) result -> maxtime ));

	fprintf(fp,
	        "Measured minimum time for this test run: %.3f us\n",
	        MICROSEC((double) result -> mintime ));

	fprintf(fp, "Measured mean for this test run: %.3f us \n", MICROSEC((double) result -> meantime ));

	fprintf(
	        fp,
	        "Measured standard deviation for this test run: %.3f us\n",
	        MICROSEC((double) result -> stdtime ));

	fprintf(fp, "Total number of conforming iterations: %d \n",
	        result -> iterations - result -> non_conforming_iterations);

	fprintf(fp, "Total number of outlying iterations: %d \n",
	        result -> non_conforming_iterations);

	//Measured execution times in the report should show up for -v:simple
	if (verbose_level >= 1) {
		long long *values = result -> values;
		int k = 0;
		fprintf(fp, "  \n");
		fprintf(fp, "Measured execution times:\n\n");

		for (k = 0; k < result -> num_entries; k++) {
			fprintf(fp,
			        "Execution time %d: %.3f us\n",
			        k, MICROSEC(values[k]));
		}
	}
}

void dump_score(vector_t *score_vec, FILE *fp, benchmark_test* bmtest,
        int verbose_level) {
	test_score_t* score;
	int score_len = 0;
	int result_length = 0;
	vector_t* result_vec = NULL;
	test_results_t* result = NULL;

	int i = 0, j = 0;
	score_len = score_vec -> size;

	for (i = 0; i < score_len; i++) {
		score = get_element_at(score_vec, i);
		result_vec = score -> result;
		result_length = result_vec -> size;

		if (score_len > 1) {
			fprintf(fp, "\n Scores for subtest: %s\n",
			        score -> name);
		}

		fprintf(fp, "\nMaximum throughput score  : %.3f\n",
		        score ->max_throughput);
		fprintf(fp, "Minimum throughput score   : %.3f\n",
		        score ->min_throughput);
		fprintf(fp, "Mean throughput score      : %.3f\n",
		        score ->mean_throughput);
		fprintf(fp, "Weighted throughput score  : %.3f\n",
		        score ->wgt_throughput);

		if (score_len > 1) {
			fprintf(fp, " Score for determinism for this "
				"subtest: %.3f\n", score ->consistency_score);
		}

		for (j = 0; j < result_length; j++) {
			result = (test_results_t *) get_element_at(result_vec,
			        j);
			fprintf(fp, "\n");
			if (result_length > 1) {
				fprintf(fp, "Test results for : %s \n",
				        result -> name);
				fprintf(fp, "======================="
					"========================\n");
			}

			print_result(fp, result, verbose_level);
		}
	}
}
