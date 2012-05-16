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
 
/* All the includes and defines go here */

#ifndef __RD_CMD_LN__
#define __RD_CMD_LN__

#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/stat.h>

#include <RTMB_common.h>

typedef enum real_time_mode {
	SOFT_RT, HARD_RT, GUARANTEED_RT
} real_time_mode_t;

typedef struct _test_name_node {
	char *_test_name;
	struct _test_name_node *_next;
} test_name_node;

/* All global datastructures go here */

typedef struct _cmd_line_args {
	int _verbose; /* Verbose output needed or not */
	int _verbose_lvl; /* Level of 'verbosity' */
	int _quick; /* Run tests for multiple iterations for compliance testing */
	int _rt_mode; /* Whether soft or hard RT mode */
	char *_file_name; /* Input filename */
	char _dir_name[MAXPATHLEN + 1]; /* results directory name */
	test_name_node *_test_list; /* Names of the test funcs */
} cmd_line_args;

/* All function declarations wrt cmd line parsing go here */

int parse_cmd_line(cmd_line_args *, int argc, char *argv[]);
void insert_in_test_list(cmd_line_args *, test_name_node *);
int setup_testcase_names(cmd_line_args *, char *);
int setup_default_options(cmd_line_args *, char *);
void cmd_line_parser_cleanup(cmd_line_args *);
void print_selected_options(cmd_line_args *);
void print_tests_list();
void usage();

#endif /* __RD_CMD_LN__ */
