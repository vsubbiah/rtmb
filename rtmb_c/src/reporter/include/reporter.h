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
 
#ifndef REPORTER_H_
#define REPORTER_H_

#include "RTMB_print.h"
#include "result_mgmt.h"
#define SYSTEM_REPORT_FILE "system_report.txt"

char* get_output_dir(char *report_dir);
void dump_result_table_entry(char *report_dir, result_table_t* result_record,
                int verbose_level);
void print_result(FILE *fp, test_results_t* result,
                int verbose_level);
void dump_score(vector_t *score_vec, FILE *fp, benchmark_test* bmtest,
                int verbose_leve);

#endif /* REPORTER_H_ */
