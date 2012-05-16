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
 
#include <dlfcn.h>
#include <string.h>

#include <RTMB_common.h>
#include <RTMB_parse_in_file.h>
#include <RTMB_rd_cmd_line.h>
#include <result_mgmt.h>

/* For characters 'lib', '.so' etc */
#define LIB_PREFIX_SUFFIX_SIZE 16

typedef struct thread_args {
	cmd_line_args *cmd_args;
	benchmark_test *bmtest;
	int ret;
	result_table_t *res_table;
} thread_args_t;

int launch_tests(cmd_line_args *cmd_args, input_params *ifl_params,
        char *home_dir, result_table_t* res_table);

int run_test(result_table_t *res_table, cmd_line_args *, benchmark_test *,
        char *);

void print_scores(benchmark_test* bmtest, result_table_t* res_table,
        cmd_line_args* cmd_args);
