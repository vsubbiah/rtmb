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
 
#ifndef __PTHREAD_WRAPPER__
#define __PTHREAD_WRAPPER__

#include <pthread.h>
#include <sched.h>

class PthreadWrapper {
public:
	static int setPthreadAttrSchedParam (pthread_attr_t *, int, int);
	static void setSchedParam (pthread_t, int, int);
	static int schedGetPriorityMax (int policy);
};

#endif /*__PTHREAD_WRAPPER__*/
