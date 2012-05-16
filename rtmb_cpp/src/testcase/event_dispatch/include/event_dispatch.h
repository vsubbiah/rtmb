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
 
#ifndef EVENT_DISPATCH_H
#define EVENT_DISPATCH_H

#ifndef EVENT_DISPATCH_PUBLIC
#define EVENT_DISPATCH_PUBLIC extern
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

bool doRun(pthread_t *signalSender, pthread_t *signalReceiver,
      TestResults * result, int subIterions, int iterations);

extern "C" bool event_dispatch(CmdLineParams &clParams,
      BenchMarkTestInfo &testInfo, RealTimeValues &rtValues,
      ResultTable &resultTable);
/* synchronization primitives to sync signal sender and receiver threads.
 * ready lock/condn is used so that receiver thread is on  conditional wait  before sender is ready to signal
 */
EVENT_DISPATCH_PUBLIC pthread_mutex_t readyLock;
EVENT_DISPATCH_PUBLIC pthread_cond_t readyCondn;
/* synchronization primitives used by signal sender thread to signal receiver thread */
EVENT_DISPATCH_PUBLIC pthread_mutex_t waitNotifyLock;
EVENT_DISPATCH_PUBLIC pthread_cond_t waitNotifyCondn;

void initializeEventDispatch();
void * signalSenderStart(void *arg);
void * signalReceiverStart(void *arg);
void cleanupEventDispatch();

int startThread(pthread_t *thread, void *(*func)(void *), void *arg);

EVENT_DISPATCH_PUBLIC long long idealWorkQuantumTime;
EVENT_DISPATCH_PUBLIC long long *threadSwitchTime;
EVENT_DISPATCH_PUBLIC int evtDispatchSubIterations;
EVENT_DISPATCH_PUBLIC int evtDispatchMajorIterations;

#endif
