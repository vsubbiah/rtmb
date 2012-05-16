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
 
/*
 * This file contains functions that are specific to parsing input file.
 */

#include <RTMB_parse_in_file.h>
#include <RTMB_print.h>
#include <errno.h>
/**
 * Name: parse_input_file()
 * Desc: Input file parser implementation.
 * Args:
 *	   input_file = Path to the input file as mentioned in cmd_args.
 *	   ifl_params = Pointer to input_params structure.
 * Rets: 0 on success and -1 on error.
 */

int parse_input_file(char *input_file, input_params *ifl_params) {

	FILE *fp;
	benchmark_test *new_test_entry;
	char input_line[MAX_INPUT_LINE_SIZE];
	int len = 0, newtest = 0;

	/* Open the input file */

	fp = fopen(input_file, "r");

	if (fp == NULL) {
		RTMB_printf(stderr, "ERROR: Error opening input file %s\n",
		        input_file);
		return ERROR;
	}

	/* Read line by line and grep each time */

	while (fgets(input_line, MAX_INPUT_LINE_SIZE, fp) != NULL) {
		/* A new line char is also included. Remove it from
		 * the input string
		 */

		len = strlen(input_line);

		if (input_line[len - 1] == '\n')
			input_line[len - 1] = '\0';

		/* Compare with what we expect the input file to contain */

		if (input_line[0] == '#') {
			/* comment - ignore */
			continue;
		} else if (input_line[0] == '\n') {
			/* newline - ignore */
			continue;
		} else if (input_line[0] == ' ') {
			/* white space - ignore */
			continue;
		} else if (strcmp(input_line, "test") == 0) {
			/* New test entry begins */
			if (newtest == 1) {
				RTMB_printf(stderr,
				        "ERROR: Bad formatting in the input file\n");
				return ERROR;
			}
			newtest = 1;
			/* allocate memory for this test */
			if ((new_test_entry
			        = (benchmark_test *) alloc_new_benchmark_test())
			        == NULL) {
				return ERROR;
			}
		} else if (strcmp(input_line, "/test") == 0) {
			/* Test entry done */
			if (newtest == 0) {
				RTMB_printf(stderr,
				        "ERROR: Bad formatting in the input file\n");
				return ERROR;
			}
			newtest = 0;
			new_test_entry->_inp_params = ifl_params;
			/* New test initialized, add to the list */
			insert_new_test_entry(ifl_params, new_test_entry);

		} else if (strncmp(input_line, "name", 4) == 0) {
			/* test name */
			assert(newtest == 1);
			if (set_input_params(new_test_entry, input_line, NAME)
			        == ERROR) {
				return ERROR;
			}
		} else if (strncmp(input_line, "desc", 4) == 0) {
			/* test desc */
			assert(newtest == 1);
			if (set_input_params(new_test_entry, input_line, DESC)
			        == ERROR) {		        
				return ERROR;
			}
		} else if (strstr(input_line, "threshold") != NULL) {
			/* Threshold to stop retrying for tighter deviation */
			assert(newtest == 1);
			if (set_input_params(new_test_entry, input_line,
			        THRESHOLD) == ERROR) {	        
				return ERROR;
			}
		} else if (strstr(input_line, "reference_score") != NULL) {
			assert(newtest == 1);
			if (set_input_params(new_test_entry, input_line,
			        REFERENCE_SCORE) == ERROR) {		        
				return ERROR;
			}
		} else if (strncmp(input_line, "iterations", 10) == 0) {
			/* test iterations */
			assert(newtest == 1);
			if (set_input_params(new_test_entry, input_line,
			        ITERATIONS) == ERROR) {			        
				return ERROR;
			}
		} else if (strncmp(input_line, "config_test", 11) == 0) {
			/* test iterations */
			assert(newtest == 1);
			if (set_input_params(new_test_entry, input_line,
			        CONFIG_TEST) == ERROR) {
				return ERROR;
			}
		} else if (strstr(input_line, "soft.acceptable_percentage")
		        != NULL) {
			if (set_exp_values(ifl_params, input_line,
			        SRT_ACCEPTABLE_PERCENT) == ERROR) {
				return ERROR;
			}
		} else if (strstr(input_line, "soft.range_percentage") != NULL) {
			/* Hard realtime outliers */
			if (set_exp_values(ifl_params, input_line,
			        SRT_RANGE_PERCENT) == ERROR) {
				return ERROR;
			}
		} else if (strstr(input_line, "soft.multiplier") != NULL) {
			/* Soft realtime outliers amount */
			if (set_exp_values(ifl_params, input_line,
			        SRT_MULTIPLIER) == ERROR) {
				return ERROR;
			}
		} else if (strstr(input_line, "hard.acceptable_percentage")
		        != NULL) {
			if (set_exp_values(ifl_params, input_line,
			        HRT_ACCEPTABLE_PERCENT) == ERROR) {
				return ERROR;
			}
		} else if (strstr(input_line, "hard.range_percentage") != NULL) {
			/* Hard realtime outliers */
			if (set_exp_values(ifl_params, input_line,
			        HRT_RANGE_PERCENT) == ERROR) {
				return ERROR;
			}
		} else if (strstr(input_line, "hard.multiplier") != NULL) {
			/* Soft realtime outliers amount */
			if (set_exp_values(ifl_params, input_line,
			        HRT_MULTIPLIER) == ERROR) {
				return ERROR;
			}
		} else if (strstr(input_line,
		        "guaranteed.acceptable_percentage") != NULL) {
			if (set_exp_values(ifl_params, input_line,
			        GRT_ACCEPTABLE_PERCENT) == ERROR) {
				return ERROR;
			}
		} else if (strstr(input_line, "guaranteed.range_percentage")
		        != NULL) {
			/* Hard realtime outliers */
			if (set_exp_values(ifl_params, input_line,
			        GRT_RANGE_PERCENT) == ERROR) {
				return ERROR;
			}
		} else if (strstr(input_line, "guaranteed.multiplier") != NULL) {
			/* Soft realtime outliers amount */
			if (set_exp_values(ifl_params, input_line,
			        GRT_MULTIPLIER) == ERROR) {
				return ERROR;
			}
		} else {
		RTMB_printf(stderr, "ERROR: Unexpected line in input file\n%s\n",
		        input_line);
			return ERROR;
		}

	}

	return 0;
}

/**
 * name: insert_new_test_entry()
 * desc: inserts a newly allocated test entry into the tests_list linked list in cmd_args structure.
 * Args:
 *	   ifl_params	 = Pointer to input_params structure
 *	   new_test_entry = A new test entry.
 * rets: None.
 */

void insert_new_test_entry(input_params *ifl_params,
        benchmark_test *new_test_entry) {
	benchmark_test *tmp;
	tmp = ifl_params->_tests;
	ifl_params->_tests = new_test_entry;
	new_test_entry->_next = tmp;
}

/*
 * Name    : set_exp_values ()
 * Desc    : Sets the expected percentages for distribution, distribution
 amount, outliers and outliers amount.
 * Args    : ifl_params = Pointer to input_params structure.
 input_line = String representing the input line from the config file.
 vals       = Enum that indicates the value to be set.
 * Returns : void.
 */

int set_exp_values(input_params *ifl_params, char *input_line, exp_vals_t vals) {
	char *key_values;
	key_values = (char *) strtok(input_line, "=");

	if (key_values == NULL) {
		RTMB_printf(stderr, "ERROR: Empty line in the input file\n");
		return ERROR;
	}

	key_values = (char *) strtok(NULL, "=");
	if (key_values == NULL) {
		RTMB_printf(stderr, "ERROR: Expected percentage"
			" not set correctly\n");
		return ERROR;
	}

	switch (vals) {
	case SRT_ACCEPTABLE_PERCENT:
		ifl_params->srt.acceptable_percentage = atof(key_values);
		if (ifl_params->srt.acceptable_percentage == 0) {
			RTMB_printf(stderr,
			        "WARNING: Possible improper value for ");
			RTMB_printf(stderr,
			        "soft realtime acceptable percentage value\n");
		}
		break;

	case SRT_RANGE_PERCENT:
		ifl_params->srt.range_percentage = atof(key_values);
		if (ifl_params->srt.range_percentage == 0) {
			RTMB_printf(stderr,
			        "WARNING: Possible improper value for ");
			RTMB_printf(stderr,
			        "soft realtime range percentage value\n");
		}
		break;

	case SRT_MULTIPLIER:
		ifl_params->srt.multiplier = atof(key_values);
		if (ifl_params->srt.multiplier == 0) {
			RTMB_printf(stderr,
			        "WARNING: Possible improper value for ");
			RTMB_printf(stderr,
			        "soft realtime multiplier value\n");
		}
		break;

	case HRT_ACCEPTABLE_PERCENT:
		ifl_params->hrt.acceptable_percentage = atof(key_values);
		if (ifl_params->hrt.acceptable_percentage == 0) {
			RTMB_printf(stderr,
			        "WARNING: Possible improper value for ");
			RTMB_printf(stderr,
			        "hard realtime acceptable percentage value\n");
		}
		break;

	case HRT_RANGE_PERCENT:
		ifl_params->hrt.range_percentage = atof(key_values);
		if (ifl_params->hrt.range_percentage == 0) {
			RTMB_printf(stderr,
			        "WARNING: Possible improper value for ");
			RTMB_printf(stderr,
			        "hard realtime range percentage value\n");
		}
		break;

	case HRT_MULTIPLIER:
		ifl_params->hrt.multiplier = atof(key_values);
		if (ifl_params->hrt.multiplier == 0) {
			RTMB_printf(stderr,
			        "WARNING: Possible improper value for ");
			RTMB_printf(stderr,
			        "hard realtime multiplier percentage value\n");
		}
		break;

	case GRT_ACCEPTABLE_PERCENT:
		ifl_params->grt.acceptable_percentage = atof(key_values);
		if (ifl_params->grt.acceptable_percentage == 0) {
			RTMB_printf(stderr,
			        "WARNING: Possible improper value for ");
			RTMB_printf(stderr,
			        "guaranteed realtime acceptable percentage"
				        " value\n");
		}
		break;

	case GRT_RANGE_PERCENT:
		ifl_params->grt.range_percentage = atof(key_values);
		if (ifl_params->grt.range_percentage == 0) {
			RTMB_printf(stderr,
			        "WARNING: Possible improper value for ");
			RTMB_printf(stderr,
			        "hard realtime range percentage value\n");
		}
		break;

	case GRT_MULTIPLIER:
		ifl_params->grt.multiplier = atof(key_values);
		if (ifl_params->grt.multiplier == 0) {
			RTMB_printf(stderr,
			        "WARNING: Possible improper value for ");
			RTMB_printf(stderr,
			        "hard realtime multiplier percentage value\n");
		}
		break;
	}

	return 0;
}

/**
 * name: set_input_params()
 * desc: Sets the name, desc, iterations and other input parameters.
 * args:
 *	   test	   = Pointer to benchmark_test.
 *	   input_line = A single line from the input file.
 *	   param	  = ENUM value that indicates the input param that should be set.
 * rets: 0 on success and -1 on fail.
 */

int set_input_params(benchmark_test *test, char *input_line, param_t param) {
	char *key_values;
	key_values = (char *) strtok(input_line, "=");
	if (key_values != NULL) {
		key_values = (char *) strtok(NULL, "=");
		switch (param) {
		case NAME:
			strcpy(test->_name, key_values);
			break;
		case DESC:
			strcpy(test->_desc, key_values);
			break;
		case ITERATIONS:
			test->_iterations = atoi(key_values);
			if (test->_iterations == 0) {
				RTMB_printf(stderr,
				        "WARNING: Possible improper value ");
				RTMB_printf(stderr,
				        "for iterations in input file\n");
			}
			break;
		case CONFIG_TEST:
			errno = 0;
			test->_config_test = atoi(key_values);
			if (test->_config_test == 0 && errno != 0) {
				RTMB_printf(stderr,
				        "WARNING: Possible improper value ");
				RTMB_printf(stderr,
				        "for config_test in input file\n");
			}
			break;
		case THRESHOLD:
			test->_threshold = atoi(key_values);
			if (test->_threshold == 0) {
				RTMB_printf(stderr,
				        "WARNING: Possible improper ");
				RTMB_printf(stderr,
				        "value for hard realtime threshold\n");
			}
			break;
		case REFERENCE_SCORE:
			test -> _reference_score = strtod(key_values, NULL);

			if (test ->_reference_score == 0) {
				RTMB_printf(stderr,
				        "WARNING: Possible improper ");
				RTMB_printf(stderr,
				        "value for reference_score\n");
				return ERROR;
			}
		}
		return 0;
	} else {
		RTMB_printf(stderr,
		        "ERROR: Bad formatting in the input file\n");
		return ERROR;
	}
}

/**
 * name: alloc_new_benchmark_test()
 * desc: Allocates memory for a new benchmark_test node.
 * args: None.
 * rets: Address of newly allocated benchmark_test structure or NULL if allocation fails.
 */

benchmark_test *alloc_new_benchmark_test() {
	benchmark_test *tmp;
	tmp = (benchmark_test *) calloc(1, sizeof(benchmark_test));
	if (tmp == NULL) {
		RTMB_printf(stderr,
		        "ERROR: malloc failed while allocating memory ");
		RTMB_printf(stderr, "in alloc_new_benchmark_test()\n");
		abort();
	}

	tmp ->_config_test = 0;
	tmp ->_next = NULL;
	return tmp;
}

/**
 * name: inputfile_parser_cleanup()
 * desc: free()s all the memory allocated during input file parsing.
 * args: pointer to input_params structure.
 * rets: void
 */
void inputfile_parser_cleanup(input_params *ifl_params) {
	benchmark_test *tmp;
	benchmark_test *prev_tmp;

	if (ifl_params) {
		tmp = ifl_params->_tests;

		while( tmp != NULL ) {
			prev_tmp = tmp;
			tmp = tmp->_next;
			free(prev_tmp);
		}

		free(ifl_params);
	}
}
