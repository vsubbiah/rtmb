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
 
package com.ibm.realtime.rtmb.tests.impl;

import com.ibm.realtime.rtmb.params.RealTimeValues;
import com.ibm.realtime.rtmb.params.TestInfo;
import com.ibm.realtime.rtmb.params.TestParams;
import com.ibm.realtime.rtmb.tests.util.BenchMarkTestThread;
import com.ibm.realtime.rtmb.tests.util.BenchMarkThread;
import com.ibm.realtime.rtmb.tests.util.PriorityType;
import com.ibm.realtime.rtmb.tests.util.RTSJCheck;
import com.ibm.realtime.rtmb.tests.util.SpecTest;
import com.ibm.realtime.rtmb.tests.util.StdBenchMarkTestThread;
import com.ibm.realtime.rtmb.tests.util.StdBenchMarkThread;
import com.ibm.realtime.rtmb.tests.util.RTSJSpecificTestUtils.RTBenchMarkTestThread;
import com.ibm.realtime.rtmb.tests.util.RTSJSpecificTestUtils.RTBenchMarkThread;

public class BenchMarkThreadFactory {
	private static BenchMarkThreadFactory instance = new BenchMarkThreadFactory();
	
	public static  BenchMarkThreadFactory getInstance() {
		return instance;
	}
	
	public BenchMarkThread getThread (PriorityType priority, Runnable logic) {
		if (RTSJCheck.checkIfRTSJ()) {
			RTBenchMarkThread rtThread = new RTBenchMarkThread (priority, logic);
			return rtThread;
		} else {
			StdBenchMarkThread stdThread = new StdBenchMarkThread (priority, logic);
			return stdThread;
		}
	}
	
	public BenchMarkTestThread getThread (PriorityType priority, SpecTest testToRun,
			TestInfo ti, RealTimeValues rtv, TestParams tp) {
		if (RTSJCheck.checkIfRTSJ()) {
			RTBenchMarkTestThread rtThread = new RTBenchMarkTestThread (priority, 
					testToRun, ti, rtv, tp);
			return rtThread;
		} else {
			StdBenchMarkTestThread stdThread = new StdBenchMarkTestThread (priority,
					testToRun, ti, rtv, tp);
			return stdThread;
		}
	}
}
