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
 
#ifndef __THR_PRIO_TEST__
#define __THR_PRIO_TEST__

#include <pthread.h>
#include <unistd.h>

#define SUCCESS 0
#define ERROR -1
#define HIGHEST_PRIO_VAL sched_get_priority_max(SCHED_FIFO)
#define HIGH_PRIO_VAL HIGHEST_PRIO_VAL-10
#define LOW_PRIO_VAL sched_get_priority_min(SCHED_FIFO)
#define MED_PRIO_VAL ( (HIGH_PRIO_VAL + LOW_PRIO_VAL) / 2 )
#define FREE_IF_NOT_NULL(x)  if((x)) free((x)) 

int priorityValues[3]; 

typedef enum threadSet {
	LOW_PRIORITY,
	MED_PRIORITY,
	HIGH_PRIORITY,
	MAX_PRIO_TYPES
} threadSetType;

typedef struct {
	int 		type;
	int 		opernAmount;
	int 		iterations;
	pthread_attr_t 	*attr;
	int 		policy;
	int 		onlineCpus;
	TestResults  *results;
	TestResults  *execTimeResults;
} thrArgs;

typedef struct {
	pthread_t	tid;
} threadId;
	
typedef struct {
	pthread_mutex_t mutex;
	pthread_cond_t cond;
} threadSyncers;

int  		getNumberOfOnlineCpus();
int CreateStartThreads ( pthread_attr_t *, int , thrArgs*, TestResults*, TestResults *, int , int ,int); 
void 		lockIncCounter();
void 		lockDeccounter();
void 		*_start (void *);
void 		cleanup (thrArgs *, thrArgs *, thrArgs *,
			pthread_attr_t *, pthread_attr_t *, pthread_attr_t *);
void 		*thread_priority (void *);
#endif /*__THR_PRIO_TEST__*/
