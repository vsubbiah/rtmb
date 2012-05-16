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

public class TestFinalDefinitions {
	public static final int CLK_CONFIG_ARR_SIZE = 200; //a value that takes ~2-4 us initially on a typical machine.
	public static final int TIMER_CONFIG_NSLEEP_TIME = 2000; // this is 2 us
	public static final int DISKIO_CONFIG_BSIZE = 1024; // bytes
	public static final int NETIO_CONFIG_BSIZE = 1024; // bytes
	public static final int NETIO_CONFIG_SVR_PORT = 1703;
	public static final int INTRATE_CONFIG_NUM = 200;
	public static final int FLRATE_CONFIG_NUM = 200;
	public static final int MCACHE_CONFIG_ARR_SIZE = 64 * 1024 * 1024;
	public static final int NETIO_CONFIG_SOCKET_TIMEOUT = 10000; // in ms
}
