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

import com.ibm.realtime.rtmb.params.RealTimeValues;
import com.ibm.realtime.rtmb.params.TestInfo;
import com.ibm.realtime.rtmb.params.TestParams;

public class StdBenchMarkTestThread extends BenchMarkTestThread implements Runnable {
	private Thread stdThread; 

	public StdBenchMarkTestThread(PriorityType priority, SpecTest testToRun, 
			TestInfo ti, RealTimeValues rtv, TestParams tp) {
		super (priority, testToRun, ti, rtv, tp);
		stdThread = new Thread (this);
		switch (priority) {
		case LowPriority:
			stdThread.setPriority(Thread.MIN_PRIORITY);
			break;
		case MedPriority:
			stdThread.setPriority((Thread.MIN_PRIORITY + Thread.MAX_PRIORITY) / 2);
			break;
		case HighPriority:
			stdThread.setPriority(Thread.MAX_PRIORITY);
		}
	}
	
	@Override
	public void waitForThreadExit() throws InterruptedException {
		stdThread.join();		
	}
	@Override
	public void execute() {
		stdThread.start();
	}
}
