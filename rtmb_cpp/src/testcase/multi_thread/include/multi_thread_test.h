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

typedef enum thread_set_type {
	LOW_PRIORITY, MED_PRIORITY, HIGH_PRIORITY, MAX_PRIO_TYPES
} thread_set_type_t;

typedef struct thr_args {
	int type;
	int opern_amount;
	int iterations;
	pthread_attr_t *attr;
	int policy;
	int online_cpus;
	TestResults *results;
	long long executionTime;
	ConfigParams *cfgParams;
} thr_args_t;

typedef struct thread_id {
	pthread_t tid;
} thread_id_t;

typedef struct thread_syncers {
	pthread_mutex_t mutex;
	pthread_cond_t cond;
} thread_syncers_t;

int getOnlineCpus();
void lockIncCounter();
void lockDecCounter();
void *_start(void *);
void cleanup(thr_args_t *high_prio_args, pthread_attr_t *high_prio_attr);
int createStartThreads(pthread_attr_t *attr, int priority, thr_args_t* targs,
        TestResults* tResult, int startIndex, int endIndex,
        ConfigParams *cfgParams);
static void sampleRun(ConfigParams& cfgParams,long long clock);
static list<TestScore *> *generateScoresForMultiThread(
        list<TestResults *> *listResult, double referenceScore, int onlineCpus);
bool runMultiThreadTest(int totalThrs, TestResults *tResult,
	TestResults &consolidatorResult,
	RealTimeValues &rtValues,CmdLineParamsBase &clParams,
	ConfigParams &cfgParams,ResultTable &resTable);

#endif /*__MULTI_THREAD_TEST__ */
