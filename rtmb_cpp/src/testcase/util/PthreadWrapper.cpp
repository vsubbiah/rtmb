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
 
#include <PthreadWrapper.h>
#include <stdio.h>

int PthreadWrapper::setPthreadAttrSchedParam(pthread_attr_t *attr, int pol,
      int prio) {
   int detS;
   detS = PTHREAD_EXPLICIT_SCHED;

   if (pthread_attr_setinheritsched(attr, detS) != 0) {
      perror("setPthreadAttrSchedParam");
      return -1;
   }

   struct sched_param param;
   param.sched_priority = prio;
   if (pthread_attr_setschedparam(attr, &param) != 0) {
      perror("setPthreadAttrSchedParam");
      return -1;
   }

   if (pthread_attr_setschedpolicy(attr, pol) != 0) {
      perror("setPthreadAttrSchedParam");
      return -1;
   }

   return 0;
}

void PthreadWrapper::setSchedParam(pthread_t tid, int policy, int prio) {
   struct sched_param param;
   param.sched_priority = prio;
   if (pthread_setschedparam(tid, policy, &param) != 0) {
      perror("setSchedParam");
   }
}

int PthreadWrapper::schedGetPriorityMax(int policy) {
   return sched_get_priority_max(policy);
}
