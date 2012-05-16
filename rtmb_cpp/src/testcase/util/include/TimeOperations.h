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
 
#ifndef __TIME_OPERNS__
#define __TIME_OPERNS__

#include <time.h>

class TimeOperations {
public:
	static long long convertTimeToNsec (struct timespec);
	static void getCurTime (struct timespec *);
	static long long getTimeDiff (struct timespec, struct timespec);
	static bool doNanoSleep (int);
	static bool doNanoSleep (long long);
}; 

#endif /*__TIME_OPERNS__*/
