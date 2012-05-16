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
 
package com.ibm.realtime.rtmb.tests.util;

public enum TestId { SYSCLOCK_CONFIG, SYSTIMER_CONFIG, IRATE_CONFIG,
		FRATE_CONFIG, MCACHE_CONFIG, DISKIOREAD_CONFIG, DISKIOWRITE_CONFIG,
		NETIO_CONFIG, TIME_ACCURACY_TEST, EVENT_DISPATCH_TEST, 
		THREAD_PRIORITY_TEST, LOCK_CONSISTENCY_TEST, GARBAGE_GENERATION_TEST, 
		COMPILATION, BYTECODE_CONSISTENCY_TEST, PERIODIC_EVENTS_TEST, 
		ASYNC_EVENTS_TEST, DISK_IO_TEST, NET_IO_TEST, MULTI_THREAD_TEST, 
		MULTI_CPU_TEST, NHRT_SUPPORT_TEST, SCOPE_ACCESS_TEST, DUMMY_TEST
}
