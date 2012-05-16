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

#include <RTMB_common.h>
#include <RTMB_launch_tests.h>
#include <RTMB_print.h>
#include <thread_oprn.h>

/**
 * Name:	launch_tests()
 * Desc:	Launch each test based on user supplied arguments.
 * args:
 * returns: 0 on PASS and -1 on FAIL
 */
#define HIGHEST_PRIO_VAL sched_get_priority_max(SCHED_FIFO)
#define HIGH_PRIO_VAL HIGHEST_PRIO_VAL-10

void get_iters(benchmark_test* bmtest, cmd_line_args* cmd_args) {
	int new_iters = bmtest -> _iterations;

	/* If we are running quick mode, then we should just do 1 iteration */
	if (cmd_args->_quick == 1) {
		new_iters = 1;
	} else {
		/*
		 * Set the appropriate number of iterations for hard
		 * and soft RT
		 */
		if (cmd_args->_rt_mode == HARD_RT) {
			new_iters *= bmtest->_inp_params->hrt.multiplier;
		} else if (cmd_args->_rt_mode == SOFT_RT) {
			new_iters *= bmtest->_inp_params->srt.multiplier;
		} else if (cmd_args->_rt_mode == GUARANTEED_RT) {
			new_iters *= bmtest->_inp_params->grt.multiplier;
		}
	}

	bmtest->_iterations = new_iters;
}

int launch_tests(cmd_line_args *cmd_args, input_params *ifl_params,
        char *home_dir, result_table_t* res_table) {
	int found = FALSE, rc = 0;
	test_name_node *i;
	benchmark_test *j;

	if (res_table == NULL) {
		RTMB_printf(stderr, "** PANIC ** malloc() failed while ");
		RTMB_printf(stderr,
		        "allocating result table in launch_tests()\n");
		abort();
	}
	memset((void *) res_table, 0, (sizeof(result_table_t) * MAX_TESTS));

	pthread_mutex_init(&res_table->res_table_mutex, NULL);
	pthread_mutex_init(&res_table->test_bucket_mutex, NULL);

	RTMB_verbose_printf(stdout, cmd_args, 2,
	        "Running the configuration tests...\n"
		        "==================================\n");

	/* Launch the configuration test ahead of all other tests */
	for (j = ifl_params->_tests; j != NULL; j = j->_next) {
		if (j->_config_test == 1) {
			run_test(res_table, cmd_args, j, home_dir);
		}
	}

	RTMB_verbose_printf(stdout, cmd_args, 2,
	        "Running micro-benchmarks...\n"
		        "===========================\n");

	/* Iterate to check if the test exists. If exists, launch the test */
	if (cmd_args->_test_list != NULL) {
		for (i = cmd_args->_test_list; i != NULL; i = i->_next) {
			found = FALSE;
			for (j = ifl_params->_tests; j != NULL; j = j->_next) {
				if (strcmp(i->_test_name, j->_name) == 0) {
					found = TRUE;
					rc = run_test(res_table, cmd_args, j,
					        home_dir);
				}
			}

			if (!found) {
				RTMB_printf(stderr,
				        "ERROR: Implementation of test %s"
					        " not found.", i->_test_name);
				RTMB_printf(stderr, " Skipping it...\n");
				rc = ERROR;
			}
		}
	} else {
		/*
		 * Loop around just the tests listed in input files
		 * and run 'em !
		 */
		for (j = ifl_params->_tests; j != NULL; j = j->_next) {
			if (j->_config_test != 1) {
				rc = run_test(res_table, cmd_args, j, home_dir);
			}
		}
	}

	pthread_mutex_destroy(&res_table->res_table_mutex);
	pthread_mutex_destroy(&res_table->test_bucket_mutex);

	return rc;
}

/**
 * Name:	run_test()
 * Desc:	Run the test requested.
 *		  The test is expected to be implemented in the shared object by name:
 *			  lib + <test_name> + .so
 *		  The test function implementing this test will be of name testname.
 *			  <test_name>
 * args:
 *		  cmd_args  =  Pointer to cmd_line_args structure.
 *		  test_node =  Pointer to the benchmark_test structure that needs to be run.
 *		  home_dir  =  home_dir for the tests.
 * returns: 0 on PASS and -1 on FAIL
 */

int run_test(result_table_t *res_table, cmd_line_args *cmd_args,
        benchmark_test *test_node, char *home_dir) {

	/* Separate RT thread to run the tests */
	pthread_t tid;
	thread_args_t args;

	/* Pass or fail */
	int rc = 0;

	/* Decl the handle to the shared obj and the func pointer */
	void *libhandle = NULL;
	void *(*test)(void *);
	char *libname;

	/* Get the testname */
	char *testname = test_node->_name;

	/* Malloc memory for the libname */
	libname
	        = malloc(MAXPATHLEN + LIB_PREFIX_SUFFIX_SIZE + strlen(testname));
	if (libname == NULL) {
		RTMB_printf(stderr, "ERROR: malloc failed in run_test()\n");
		abort();
	} else {
		memset(libname, 0, (MAXPATHLEN + LIB_PREFIX_SUFFIX_SIZE
		        + strlen(testname)));

		/* Set up the libname in the expected format of the library name. */
		strcpy(libname, home_dir);
		libname = strcat(libname, "/lib/");
		libname = strcat(libname, "lib");
		libname = strcat(libname, testname);
		libname = strcat(libname, ".so");
		/* Open the DL library */
		libhandle = dlopen(libname, RTLD_NOW | RTLD_GLOBAL);
		if (libhandle != NULL) {
			/* Clear any earlier errors - per dlsym man page */
			dlerror();

			/* Find the testname symbol from the library */
			test = (void *(*)(void *)) dlsym(libhandle, testname);
			if (test != NULL) {
				pthread_attr_t attr;
				args.cmd_args = cmd_args;
				args.ret = SUCCESS;
				get_iters(test_node, cmd_args);
				args.bmtest = test_node;

				args.res_table = res_table;

				pthread_attr_init(&attr);
				set_pthreadattr_sched_param(&attr, SCHED_FIFO,
				        HIGH_PRIO_VAL);
				if (pthread_create(&tid, &attr, test,
				        (void *) &args) != 0) {
					RTMB_printf(stderr,
					        "ERROR: Thread creation failed\n");
					perror("run_test");
					return ERROR;
				}
				if (pthread_join(tid, (void **) NULL) != 0) {
					RTMB_printf(stderr,
					        "ERROR: Thread join failed\n");
					perror("run_test");
					return ERROR;
				}
				if (args.ret == ERROR) {
					RTMB_printf(stderr,
					        "ERROR: Test run %s "
						        "reported errors\n",
					        testname);
					rc = ERROR;
				} else { /*  if( args.ret == SUCCESS) */
					print_scores(test_node, res_table,
					        cmd_args);
				}
			} else {
				RTMB_printf(stderr,
				        "ERROR: Function call for test"
					        " %s not found.\n", testname);
				rc = ERROR;
			}
			/* Done, close the lib handle and return */
			dlclose(libhandle);
		} else {
			RTMB_printf(stderr,
			        "ERROR: dlopen() failed for library %s. %s\n",
			        libname, dlerror());
			RTMB_printf(stderr, "Hint: Check LD_LIBRARY_PATH\n");
			rc = ERROR;
		}
	}

	fflush(stderr);
	fflush(stdout);
	/* Free memory allocated for libname and return */
	if (libname != NULL) {
		free(libname);
	}

	return rc;
}

/*
 * Name         : print_scores
 * Parameters   : user supplied argc and *argv[]
 * Returns      : 0 on success and -1 on error.
 * Desc         : main() function
 */
void print_scores(benchmark_test* bmtest, result_table_t* res_table,
        cmd_line_args* cmd_args) {

	final_bm_test_result_t* fbmt;
	int i;
	int number_of_cpus;
	char* test_name = bmtest ->_name;

	number_of_cpus = get_num_online_cpus();

	/*
	 * Do not try to print scores for tests that are not meant
	 * to be run on an uniprocessor machine.
	 */
	if (number_of_cpus == 1) {
		if ((strcmp(test_name, "thread_priority") == 0) || (strcmp(
		        test_name, "multi_thread") == 0)) {
			return;
		}
	}

	for (i = 0; i < MAX_TESTS; i++) {
		if (res_table[i].valid && (strcmp(res_table[i].bm_test->_name,
		        bmtest ->_name) == 0)) {
			break;
		}
	}

	if (i == MAX_TESTS)
		return;

	fbmt = res_table[i].fbmt;

	if (fbmt != NULL) {
		RTMB_verbose_printf(stdout, cmd_args, 1, "\nScores:\n");

		vector_t* score_vec = fbmt ->score;

		for (i = 0; i < score_vec -> size; i++) {
			test_score_t * score = get_element_at(score_vec, i);
			RTMB_verbose_printf(stdout, cmd_args, 1,
			        "Maximum Throughput   : %.3f \n",
			        score ->max_throughput);
			RTMB_verbose_printf(stdout, cmd_args, 1,
			        "Minimum Throughput   : %.3f \n",
			        score ->min_throughput);
			RTMB_verbose_printf(stdout, cmd_args, 1,
			        "Mean Throughput      : %.3f \n",
			        score ->mean_throughput);
			RTMB_verbose_printf(stdout, cmd_args, 1,
			        "Weighted Throughput  : %.3f \n",
			        score ->wgt_throughput);
			RTMB_printf(stdout, "\n");
		}

		RTMB_verbose_printf(stdout, cmd_args, 1,
		        "Overall Throughput Score for test   : %.3f \n"
			        "Overall Determinism Score for test   : %.3f"
			        " \n\n", fbmt ->throughput_test_score,
		        fbmt ->consistency_test_score);

		RTMB_verbose_printf(stdout, cmd_args, 1,
		        "Final Score for %s micro-benchmark : %.3f \n\n",
		        bmtest ->_name, fbmt -> test_score);
	}
}
