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
 * This is the main file for the RT Benchmark test suite.
 */

/* All the includes and defines specific to this file are below */
#include <RTMB_common.h>
#include <RTMB_parse_in_file.h>
#include <RTMB_rd_cmd_line.h>
#include <RTMB_launch_tests.h>
#include <sys/mman.h>
#include "reporter.h"

/* Function declarations */
void RTMB_panic(const char*, int);
char *build_home_dir(char *);
void resolve_symbolic_link(char *);

/* Function implementations start below */

/*
 * Name	   	: main()
 * Parameters 	: user supplied argc and *argv[]
 * Returns	: 0 on success and -1 on error.
 * Desc	   	: main() function
 */
int main(int argc, char *argv[]) {
	/* A pointer to the cmd_line_args data structures */
	cmd_line_args *cmd_args = NULL;

	/* Home dir. This is used to pick up the libs */
	char *home_dir = NULL;

	/* A pointer to the input_params data structures */
	input_params *ifl_params = NULL;

	result_table_t *res_table = NULL;

	/* Lock all pages of the process in memory to avoid paging */
	if (mlockall(MCL_CURRENT | MCL_FUTURE) != 0) {
		fprintf(stderr, "mlockall failed\n");
		return ERROR;
	}

	/* Check if the result mgmt table is alloced. If not, alloc it. */
	res_table
	        = (result_table_t *) calloc(sizeof(result_table_t), MAX_TESTS);

	/* Build the home directory from argv[0] */
	home_dir = (char *) build_home_dir(argv[0]);
	if (home_dir == NULL) {
		return ERROR;
	}
	printf("argv[0]=%s\n", argv[0]);
	printf("home_dir=%s\n", home_dir);

	/* Allocate memory for cmd_line_args and input_params */
	cmd_args = (cmd_line_args *) calloc(1, sizeof(cmd_line_args));
	ifl_params = (input_params *) calloc(1, sizeof(input_params));

	if (cmd_args == NULL || ifl_params == NULL) {
		fprintf(stderr, "ERROR: malloc failed while allocating memory"
			" in main()\n");
		abort();
	}

	/* Setup the default options for cmd line args */
	if (setup_default_options(cmd_args, home_dir) == ERROR) {
		RTMB_panic("Setup default options", ERROR);
	}

	/* Parse the argument list for user supplied options */
	if (parse_cmd_line(cmd_args, argc, argv) == ERROR) {
		RTMB_panic("Parse command line", ERROR);
	}

	/* Parse the input file for other input parameters */
	if (parse_input_file(cmd_args->_file_name, ifl_params) == ERROR) {
		RTMB_panic("Parse input file", ERROR);
	}

	/* Launch the tests now */
	launch_tests(cmd_args, ifl_params, home_dir, res_table);

	if (home_dir != NULL) {
		free(home_dir);
	}

	if (cmd_args ->_dir_name[0] == '\0') {
		strcpy(cmd_args ->_dir_name, "./");
	} else {
		int len = strlen(cmd_args -> _dir_name);
		if (cmd_args ->_dir_name[len - 1] != '/') {
			strcat(cmd_args -> _dir_name, "/");
		}
	}

	compute_system_scores(res_table);

	/* generate report */
	dump_result_table_entry(cmd_args -> _dir_name, res_table,
	        cmd_args->_verbose_lvl);

	RTMB_printf(stdout,
	        "\nFinal Throughput Score for the system: %.3f\n",
	        system_throughput_score);

	RTMB_printf(stdout,
	        "Final Determinism Score for the system: %.3f\n",
	        system_consistency_score);

	RTMB_printf(stdout,
	        "\nFinal Score for the system: %.3f\n",
	        system_score);

	/* Cleanup */
	cmd_line_parser_cleanup(cmd_args);
	inputfile_parser_cleanup(ifl_params);

	return 0;
}

/**
 * Name	     : RTMB_panic()
 * Desc	     : Use to exit() the program. Note that no cleanup is done and this is an abnormal exit.
 * Arguments : error_code - The error value to exit the program with.
 * Returns   : N/A.
 */
void RTMB_panic(const char* msg, int error_code) {
  puts(msg);
  printf("Panic with error_code:%d\n", error_code);
  fflush(stdout);
	exit(error_code);
}

/**
 * Name    : build_home_dir()
 * Desc    : Builds the home dir from the argv[0]
 * Args    : argv[0]
 * Retruns : 0 on success; -1 on error
 */
char *build_home_dir(char *file_name) {
	char *home_dir;
	char *path;
	char *p;
	char *p1;
	int len;
	char dir[MAXPATHLEN];
	char *found = NULL;
	struct stat s;
	mode_t mode;

	/* The binary name can be fully or partially qualified */

	if (strchr(file_name, '/') != NULL && stat(file_name, &s) == 0) {
		if (file_name[0] == '/') {
			strcpy(dir, file_name);
			found = dir;
		} else {
			getcwd(dir, MAXPATHLEN);
			path = dir;
			p = file_name;
			while (p[0] == '.') {
				if (strstr(p, "../") == p) {
					p = p + 3;
					p1 = (char *) dirname(path);
					if (path != p1)
						strcpy(path, p1);
				} else {
					p = p + 2;
				}
			}
			strcat(path, "/");
			strcat(path, p);
			found = path;
		}
	} else {
		/*
		 * If not found, assume it's a short name
		 * and search the path variable
		 */
		path = getenv("PATH");
		if (path != NULL) {
			while (path != NULL) {
				/* Get the first element of the path */
				p = strchr(path, ':');
				if (p != NULL) {

					/* There is more than one */
					len = p - path;
					strncpy(dir, path, len);
					dir[len] = 0;
					path = p + 1;
				} else {

					/* Last path element.*/
					strcpy(dir, path);
					path = NULL;
				}

				/* Add the binary name to the path element and
				 * check if it points to a file.
				 */
				strcat(dir, "/");
				strcat(dir, file_name);
				if (stat(dir, &s) == 0) {
					mode = s.st_mode & S_IFMT;
					if (mode == S_IFREG) {
						/* Found a match! */
						found = dir;
						break;
					}
				}
			}
		} else {
			fprintf(stderr,
			        "ERROR: getenv() failed in build_home_dir()\n");
		}
	}

	/* Found? */
	if (found != NULL) {

		/* Resolve symbolic link in name */
		resolve_symbolic_link(found);

		/* Strip the exc's name off the end. */
		p = (char *) dirname(found);
		if (found != p) {
			strcpy(found, p);
		}

		resolve_symbolic_link(found);

		/* Remove the next component which should be '/bin'*/
		p = (char *) dirname(found);
		if (found != p) {
			strcpy(found, p);
		}

		resolve_symbolic_link(found);

		/* Return home_dir back */
		home_dir = strdup(found);
	}

	return home_dir;
}

/**
 * Name    : resolve_symbolic_link()
 * Desc    : Resolves to the actual file if a sym link is sent as argument.
 * Args    : Path to the file.
 * Returns : void
 */
void resolve_symbolic_link(char *file) {
	char buffer[MAXPATHLEN];
	char *p = file;
	if (realpath(file, buffer)) {
		strcpy(p, buffer);
	}
}
