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
 
#ifndef __TIME_ACCURACY__
#define __TIME_ACCURACY__


#include <pthread.h>

#include <time.h>
/* From util code of tests */
#include <time_mgmt.h>
#include <result_mgmt.h>
#include <thread_oprn.h>

/* From framework code */
#include <RTMB_rd_cmd_line.h>
#include <RTMB_parse_in_file.h>
#include <RTMB_launch_tests.h>

#define SUCCESS 0
#define ERROR -1

/* All function declarations go below */
void *time_accuracy(void *);

#endif /* __TIME_ACCURACY__ */
