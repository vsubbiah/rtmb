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
 
#ifndef __THR_OPRN__
#define __THR_OPRN__

#include <pthread.h>
#include <stdio.h>

#include <RTMB_print.h>

#define SUCCESS 0
#define ERROR -1

int set_sched_param( pthread_t, int, int);
int get_sched_param( pthread_t, int *, struct sched_param *);
int set_pthreadattr_sched_param(pthread_attr_t *, int, int);
int get_pthreadattr_sched_param(const pthread_attr_t *, int *,
        struct sched_param *);
int get_num_online_cpus();

#endif /* __THR_OPRN__ */
