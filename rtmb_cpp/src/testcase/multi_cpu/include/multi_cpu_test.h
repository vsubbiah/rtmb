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
 
#ifndef __MULTI_THREAD_TEST__
#define __MULTI_THREAD_TEST__

#include <pthread.h>
#include <unistd.h>

#define SUCCESS 0
#define ERROR -1
#define HIGHEST_PRIO_VAL sched_get_priority_max(SCHED_FIFO)
#define HIGH_PRIO_VAL HIGHEST_PRIO_VAL-10
#define LOW_PRIO_VAL sched_get_priority_min(SCHED_FIFO)
#define MED_PRIO_VAL ( (HIGH_PRIO_VAL + LOW_PRIO_VAL) / 2 )
#define FREE_IF_NOT_NULL(x)  if((x)) free((x))

typedef struct {
	pid_t pid;
	bool exited;
	int priority;
	char filename[50];
} ChildProcs;

int get_num_online_cpus();
bool runMultiCpuTest(long long workTime, int onlineCpus, int newIters,
        int subIters, unsigned long timeToStart, TestResults *multiCpuResult,
        RealTimeValues &rtValues, CmdLineParamsBase &cmdLineParams);

#endif /*__MULTI_THREAD_TEST__ */
