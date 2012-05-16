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
 
#ifndef __TIME_MGMT__
#define __TIME_MGMT_

#include <time.h>
#include <stdio.h>
#include <errno.h>

#define SUCCESS 0
#define ERROR -1

long long convert_time_to_nsec(struct timespec);
int get_cur_time(struct timespec *time);
long long get_time_diff(struct timespec, struct timespec);
int do_nano_sleep(int nsecs);

#endif /* __TIME_MGMT_ */
