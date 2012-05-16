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
 
#define EVENT_DISPATCH_PUBLIC
#include <event_dispatch.h>

static volatile char signalled = FALSE;
static volatile char receiverWaiting = FALSE;

void initializeEventDispatch() {
   pthread_mutex_init(&waitNotifyLock, NULL);
   pthread_mutex_init(&readyLock, NULL);

   pthread_cond_init(&readyCondn, NULL);
   pthread_cond_init(&waitNotifyCondn, NULL);
}

void cleanupEventDispatch() {
   pthread_mutex_destroy(&waitNotifyLock);
   pthread_mutex_destroy(&waitNotifyLock);

   pthread_cond_destroy(&readyCondn);
   pthread_cond_destroy(&waitNotifyCondn);
}

void waitTillReceiverStarted() {
   /* wait till the receiver is on conditional wait */
   pthread_mutex_lock(&readyLock);

   while (!receiverWaiting) {
      pthread_cond_wait(&readyCondn, &readyLock);
   }

   receiverWaiting = FALSE;
   pthread_mutex_unlock(&readyLock);
}

void setReceiverStarted() {
   pthread_mutex_lock(&readyLock);
   receiverWaiting = TRUE;
   pthread_cond_signal(&readyCondn);
   pthread_mutex_unlock(&readyLock);
}
void *signalSenderStart(void * arg) {
   int majorIteration, subIteration = 0;
   struct timespec start_time, end_time;

   for (majorIteration = 0; majorIteration < evtDispatchMajorIterations; majorIteration++) {
      waitTillReceiverStarted();

      pthread_mutex_lock(&waitNotifyLock);
      TimeOperations::getCurTime(&start_time);
      for (subIteration = 0; subIteration < evtDispatchSubIterations; subIteration++) {
         pthread_cond_signal(&waitNotifyCondn);
         pthread_cond_wait(&waitNotifyCondn, &waitNotifyLock);
      }
      pthread_mutex_unlock(&waitNotifyLock);
      TimeOperations::getCurTime(&end_time);

      long long diffTime = TimeOperations::getTimeDiff(end_time, start_time);
      threadSwitchTime[majorIteration] = diffTime;
   }

   return NULL;
}

void *signalReceiverStart(void * arg) {
   int majorIteration, subIteration = 0;

   for (majorIteration = 0; majorIteration < evtDispatchMajorIterations; majorIteration++) {
      pthread_mutex_lock(&waitNotifyLock);
      setReceiverStarted();

      for (subIteration = 0; subIteration < evtDispatchSubIterations; subIteration++) {
         pthread_cond_wait(&waitNotifyCondn, &waitNotifyLock);
         pthread_cond_signal(&waitNotifyCondn);
      }

      pthread_mutex_unlock(&waitNotifyLock);
   }

   return NULL;
}

int startThread(pthread_t * th, void *(*func)(void*), void *arg) {
   pthread_attr_t attr;

   pthread_attr_init(&attr);

   if (PthreadWrapper::setPthreadAttrSchedParam(&attr, SCHED_FIFO,
         (PthreadWrapper::schedGetPriorityMax(SCHED_FIFO) - 10)) < 0) {
      return -1;
   }

   if (pthread_create(th, &attr, func, arg) < 0) {
      return -1;
   }

   return 0;
}
