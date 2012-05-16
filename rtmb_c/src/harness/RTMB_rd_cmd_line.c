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
 
#include <RTMB_rd_cmd_line.h>
#include <RTMB_print.h>

/*
 * Name	   : parse_cmd_line
 * Parameters : argc and argv (user supplied arguments).
 * Returns	: 0 on success and -1 on error
 * Desc	   : Command Line Parser
 */
int parse_cmd_line(cmd_line_args *cmd_args, int argc, char *argv[]) {
	int i, bad_option = FALSE;

	for (i = 1; i < argc; i++) {
		/* check for help options */
		if ((strcmp(argv[i], "--help") == 0) || (strcmp(argv[i], "-h")
		        == 0) || (strcmp(argv[i], "-?") == 0)) {
			usage(cmd_args);
			exit(0);
		} else if (strncmp(argv[i], "--verbose:simple", 16) == 0
		        || strncmp(argv[i], "-v:simple", 9) == 0) {
			cmd_args->_verbose = TRUE;
			cmd_args->_verbose_lvl = 1;
		} else if (strncmp(argv[i], "--verbose:detailed", 18) == 0
		        || strncmp(argv[i], "-v:detailed", 11) == 0) {
			cmd_args->_verbose = TRUE;
			cmd_args->_verbose_lvl = 2;
		} else if (strcmp(argv[i], "--silent") == 0 || strcmp(argv[i],
		        "-s") == 0) {
			cmd_args->_verbose = FALSE;
		} else if ((strcmp(argv[i], "--file") == 0) || strcmp(argv[i],
		        "-f") == 0) {
			/* copy over the input filename */

			if (argv[i + 1] != NULL) {
				if (strcpy(cmd_args->_file_name, argv[i + 1])
				        == NULL) {
					RTMB_printf(stderr,
					        "ERROR: Problem setting input"
						        " file\n");
					return ERROR;
				}
			} else {
				RTMB_printf(stderr,
				        "Provide a valid input filename with ");
				RTMB_printf(stderr, "--file or -f option\n");
				usage(cmd_args);
				return ERROR;
			}
			i++;
		} else if (strcmp(argv[i], "--report") == 0 || strcmp(argv[i],
		        "-r") == 0) {

			/* copy over the report directory name */

			if (argv[i + 1] != NULL) {
				if (strcpy(cmd_args->_dir_name, argv[i + 1])
				        == NULL) {
					RTMB_printf(stderr,
					        "ERROR: Problem setting report "
						        "directory\n");
					return ERROR;
				}
			} else {
				RTMB_printf(stderr,
				        "Provide a valid directory name with "
					        "--report ");
				RTMB_printf(stderr, "or -r option\n");
				usage();
				return ERROR;
			}
			i++;
		} else if (strcmp(argv[i], "--quick") == 0 || strcmp(argv[i],
		        "-q") == 0) {
			cmd_args->_quick = TRUE;
		} else if (strcmp(argv[i], "--compliant") == 0 || strcmp(
		        argv[i], "-c") == 0) {
			cmd_args->_quick = FALSE;
		} else if (strcmp(argv[i], "--test") == 0 || strcmp(argv[i],
		        "-t") == 0) {

			/* Check if some explicit tests need to be run */
			if (argv[i + 1] != NULL) {
				if (setup_testcase_names(cmd_args, argv[i + 1])
				        == ERROR) {
					return ERROR;
				}
			} else {
				RTMB_printf(stderr,
				        "Provide valid tests to be run with ");
				RTMB_printf(stderr, "--test or -t option\n");
				usage(cmd_args);
				return ERROR;
			}
			i++;
		} else if (strcmp(argv[i], "--test?") == 0 || strcmp(argv[i],
		        "-t?") == 0) {
			print_tests_list();
			exit(0);
		} else if (strcmp(argv[i], "--hard-rt") == 0) {
			cmd_args->_rt_mode = HARD_RT;
		} else if (strcmp(argv[i], "--soft-rt") == 0) {
			cmd_args->_rt_mode = SOFT_RT;
		} else if (strcmp(argv[i], "--guaranteed-rt") == 0) {
			cmd_args->_rt_mode = GUARANTEED_RT;
		} else {
			RTMB_printf(stderr, "ERROR: Unknown option '%s'\n",
			        argv[i]);
			bad_option = TRUE;
		}
	}

	if (bad_option) {
		usage(cmd_args);
		return ERROR;
	}
	print_selected_options(cmd_args);
	return 0;
}

/**
 * Name     : print_tests_list()
 * Desc	    : Displays names of all the tests supported by this benchmark.
 * Args     : Pointer to cmd_line_args structure.
 * Returns  : void
 */

void print_tests_list() {
	RTMB_printf(stdout, "Usage: rtmb [-t|--test] [<tests>]*\n");
	RTMB_printf(stdout, "Where <tests> could be one or more of:\n");
	RTMB_printf(stdout,
	        "\ntime-accuracy\t\tVerifies that the clock and timer work "
		        "consistently.\n"
		        "event-dispatch\t\tVerifies events can be dispatched"
		        " with consistent overhead.\n"
		        "thread-priority\t\tVerifies higher priority threads "
		        "preempt lower priority threads.\n"
		        "lock-consistency\tVerifies that acquiring locks "
		        "without contention runs consistently.\n"
		        "periodic-events\t\tVerifies that periodic real-time "
		        "events are dispatched consistently.\n"
		        "multi-thread\t\tVerifies that systems with multiple "
		        "cpus can scale reasonably across\n"
		        "\t\t\tmultiple application threads in a single "
		        "process.\n");
}

/**
 * Name     : print_selected_options()
 * Desc     : Prints the command line options slected for this test run.
 * Args     : Pointer to cmd_line_args structure.
 * Returns  : void.
 */

void print_selected_options(cmd_line_args *cmd_args) {
	RTMB_verbose_printf(stdout, cmd_args, 1,
	        "============================================================"
		        "========\n");
	RTMB_verbose_printf(stdout, cmd_args, 1, "Command Line options:\n");
	RTMB_verbose_printf(stdout, cmd_args, 1, "\n");
	RTMB_verbose_printf(stdout, cmd_args, 1,
	        "Input Parameter file: %s\n", cmd_args->_file_name);
	RTMB_verbose_printf(stdout, cmd_args, 1, "Report Directory: %s\n",
	        cmd_args->_dir_name);
	RTMB_verbose_printf(stdout, cmd_args, 1, "Verbose mode enabled.\n");
	if (cmd_args->_quick) {
		RTMB_verbose_printf(stdout, cmd_args, 1,
		        "Performing a quick single iteration run.\n");
	} else {
		RTMB_verbose_printf(stdout, cmd_args, 1,
		        "Performing a full compliant run.\n");
	}
	if (cmd_args->_test_list != NULL) {
		RTMB_verbose_printf(stdout, cmd_args, 1,
		        "Tests requested to be run:\n");
		test_name_node *tmp;
		int i;
		for (tmp = cmd_args->_test_list, i = 0; tmp != NULL; tmp
		        = tmp->_next, i++) {
			RTMB_verbose_printf(stdout, cmd_args, 1, "%d.\t%s\n",
			        i, tmp->_test_name);
		}
	} else {
		RTMB_verbose_printf(stdout, cmd_args, 1,
		        "No user specified tests. Running all the tests.\n");
	}
	if (cmd_args->_rt_mode == HARD_RT) {
		RTMB_verbose_printf(stdout, cmd_args, 1,
		        "Hard real-time mode option selected\n");
	} else if(cmd_args->_rt_mode == SOFT_RT) {
		RTMB_verbose_printf(stdout, cmd_args, 1,
		        "Soft real-time mode option selected\n");
	}else  {
		RTMB_verbose_printf(stdout, cmd_args, 1,
		        "Guaranteed real-time mode option selected\n");
	}
	RTMB_verbose_printf(stdout, cmd_args, 1,
	        "============================================================="
		        "=======\n");
}

/*
 * Name   : insert_in_test_list()
 * Params :
 *		  cmd_args	  = Pointer to cmd_line_args structure.
 *		  test_name_entry = Pointer to the newly allocated
 *		  test_name_node structure.
 * Returns: None.
 * Desc   : Inserts a new test_name_node into the linked list
 * 		 _test_list in cmd_args.
 */

void insert_in_test_list(cmd_line_args *cmd_args,
        test_name_node *test_name_entry) {
	test_name_node *tmp;
	tmp = cmd_args->_test_list;
	cmd_args->_test_list = test_name_entry;
	test_name_entry->_next = tmp;
}

/*
 * Name   : setup_testcase_names()
 * Params :
 *		  cmd_args  = Pointer to cmd_line_args structure.
 *		  testnames = Colon (:) separated list of user
 *		  supplied list of the tests names.
 * Returns: 0 on success and -1 on error
 * Desc   : Sets up user supplied tests. Only called when -t or
 * 		--test option is specified.
 */

int setup_testcase_names(cmd_line_args *cmd_args, char *testnames) {
	char *token;
	char *saveptr;
	test_name_node *tmp;

	if (testnames == NULL) {
		RTMB_printf(stderr,
		        "ERROR: Invalid test names argument passed\n");
		return ERROR;
	}
	token = strtok_r(testnames, ":", &saveptr);
	if (token == NULL) {
		/* single test. Alloc, insert and return */
		tmp = (test_name_node *) malloc(sizeof(test_name_node));
		if (tmp == NULL) {
			RTMB_printf(stderr, "ERROR: malloc() failed in "
				"setup_testcase_names()\n");
			abort();
		}
		tmp->_test_name = testnames;
		insert_in_test_list(cmd_args, tmp);
		return 0;
	} else {
		/* First test name. Alloc, insert and proceed to the next */
		tmp = (test_name_node *) malloc(sizeof(test_name_node));
		if (tmp == NULL) {
			RTMB_printf(stderr, "ERROR: malloc() failed in "
				"setup_testcase_names()\n");
			abort();
		}
		tmp->_test_name = token;
		insert_in_test_list(cmd_args, tmp);
	}

	while ((token = strtok_r(NULL, ":", &saveptr)) != (char *) NULL) {
		tmp = (test_name_node *) malloc(sizeof(test_name_node));
		if (tmp != NULL) {
			tmp->_test_name = token;
		} else {
			RTMB_printf(stderr, "ERROR: malloc() failed in "
				"setup_testcase_names()\n");
			abort();
		}
		insert_in_test_list(cmd_args, tmp);
	}
	return 0;
}

/* Name   : setup_default_options()
 * Desc   : Sets up the tests parameters to run in default mode.
 * Params : cmd_args = Pointer to the cmd_line_args structure.
 * Returns: '0' when success and '-1' if failed.
 */

int setup_default_options(cmd_line_args *cmd_args, char *home_dir) {
	/* Silent and compliant are the defaults. Set it up accordingly */

	cmd_args->_verbose = FALSE;
	cmd_args->_quick = FALSE;
	cmd_args->_rt_mode = SOFT_RT;

	/* Setup the path the default result/report directory. This is the
	 *  cwd */

	if (getcwd((char *) cmd_args->_dir_name, MAXPATHLEN + 1) == NULL) {
		perror("getcwd:");
		return ERROR;
	}
	cmd_args->_file_name = (char *) malloc(MAXPATHLEN + 1);
	if (cmd_args->_file_name == NULL) {
		RTMB_printf(stderr,
		        "ERROR: malloc() failed in setup_default_options()\n");
		abort();
	}
	strcpy(cmd_args->_file_name, cmd_args->_dir_name);
	cmd_args->_file_name
	        = strcat(cmd_args->_file_name, "/config/input.txt");
	return 0;
}

/* Name	  : cmd_line_parser_cleanup()
 * Desc:  : Cleans up the memory allocated for cmd line parsing
 * Params : void
 * Returns: void
 */
void cmd_line_parser_cleanup(cmd_line_args *cmd_args) {
	test_name_node *tmp;
	test_name_node *next;
	if (cmd_args) {
		tmp = cmd_args->_test_list;

		while (tmp != NULL) {
			next = tmp-> _next;
			free(tmp);
			tmp = next;
		}
		free(cmd_args);
	}
}

/*
 * Name	     : usage()
 * Desc	     : Prints the usage details of the tests
 * Parameters: cmd_args - pointer to cmd line args.
 * Returns   : void
 */

void usage() {
	RTMB_printf(stderr,
	        "\nUsage: rtmb [<option>]*, where <option> is one of:\n\n");
	RTMB_printf(stderr,
	        "-h, --help, -?\t\tPrint the command-line syntax.\n"
		        "-s, --silent\t\tSilent run (default).\n"
		        "-v, --verbose:level\tVerbose output. Opposite of -s.\n"
		        "\t\t\tWhere <level> should be one of:\n"
		        "\t\t\t    simple   = simple verbose output\n"
		        "\t\t\t    detailed = detailed verbose output\n"
		        "-f, --file <file>\tPath for input configuration"
		        " file.\n"
		        "\t\t\tDefaults to file \"$HOME/config/input.txt\"\n"
		        "-r, --report <dir>\tPath to report directory."
		        " Defaults to $PWD.\n"
		        "-q, --quick\t\tPerform a quick single iteration run.\n"
		        "-c, --compliant\t\tPerform compliant run. "
		        "Opposite of -q (default).\n"
		        "--hard-rt\t\tRun in hard real-time mode.\n"
		        "--soft-rt\t\tRun in soft real-time mode.\n"
			"--guaranteed-rt\t\tRun in guaranteed real-time mode.\n"
		        "-t, --test [<tests>]*\tTests to run separated by "
		        "colon. Default = all.\n"
		        "\t\t\tType \"-t?\" or \"--test?\" to print list of"
		        " tests.\n\n");
}
