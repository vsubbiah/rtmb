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
 
#ifndef PERIODIC_EVENT_TEST_H
#define PERIODIC_EVENT_TEST_H

#ifndef PERIODIC_EVENT_TEST_PUBLIC
#define PERIODIC_EVENT_TEST_PUBLIC extern 
#endif 

#include <pthread.h>
#include <unistd.h>


#define SUCCESS 0
#define ERROR -1
#define TRUE 1
#define FALSE 0

#include <iostream>
#include <list>
#include <pthread.h>
#include <stdio.h>
#include <cstdlib>
#include <stdlib.h>
#include <string.h>

#include <CmdLineParamsBase.h>
#include <CmdLineParams.h>
#include <InputParams.h>
#include <RTMB.h>
#include <BenchMarkTestInfo.h>
#include <TestResults.h>
#include <TestResultsUtils.h>
#include <RealTimeValues.h>
#include <ResultTable.h>

#include <PthreadWrapper.h>
#include <TimeOperations.h>
#include <DiskIOReadUtils.h>
#include <VerbosePrint.h>
#include <CmdLineParamsBase.h>
#include <config_test.h>
#define MIN_SUB_ITERATION 1 
#define FREE_IF_NOT_NULL(x) if((x)) free((x))

extern "C" bool periodic_event(CmdLineParams &clParams,
        BenchMarkTestInfo &testInfo, RealTimeValues &rtValues,
		        ResultTable &resultTable) ;

PERIODIC_EVENT_TEST_PUBLIC pthread_mutex_t readyLock;  
PERIODIC_EVENT_TEST_PUBLIC pthread_cond_t readyCond;

PERIODIC_EVENT_TEST_PUBLIC pthread_mutex_t eventLock;
PERIODIC_EVENT_TEST_PUBLIC pthread_cond_t eventCond;

typedef struct periodic_event {
    struct timespec period_start;
    struct timespec period;
    int events;
} periodic_event_t;


void initializePeriodicEvents(  );
void * eventTimerStart( void *arg  );	
void * eventHandlerStart( void *arg );

int startThread (pthread_t *thread, void *(*func) (void *), void *arg);

PERIODIC_EVENT_TEST_PUBLIC long long *eventReceiveTime;

#endif
