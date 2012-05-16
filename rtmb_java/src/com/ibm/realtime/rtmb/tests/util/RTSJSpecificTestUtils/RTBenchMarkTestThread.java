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
 
package com.ibm.realtime.rtmb.tests.util.RTSJSpecificTestUtils;

import javax.realtime.PriorityParameters;
import javax.realtime.RealtimeThread;

import com.ibm.realtime.rtmb.params.RealTimeValues;
import com.ibm.realtime.rtmb.params.TestInfo;
import com.ibm.realtime.rtmb.params.TestParams;
import com.ibm.realtime.rtmb.tests.util.BenchMarkTestThread;
import com.ibm.realtime.rtmb.tests.util.PriorityType;
import com.ibm.realtime.rtmb.tests.util.SpecTest;
import com.ibm.realtime.rtmb.util.RTSJSpecificTest;
import com.ibm.realtime.rtmb.util.RTSJSpecificUtils.RTMBPriorityScheduler;

@RTSJSpecificTest
public class RTBenchMarkTestThread extends BenchMarkTestThread implements Runnable {
	private RealtimeThread rtThread;
	
	/* (non-Javadoc)
	 * @see com.ibm.realtime.rtmb.tests.util.BenchMarkTestThread#join()
	 */
	@Override
	public void waitForThreadExit() throws InterruptedException {
		rtThread.join();
	}

	/* (non-Javadoc)
	 * @see com.ibm.realtime.rtmb.tests.util.BenchMarkTestThread#start()
	 */
	@Override
	public void execute() {
		rtThread.start();
	}

	public RTBenchMarkTestThread (PriorityType priority, SpecTest testToRun,
			TestInfo ti, RealTimeValues rtv, TestParams tp) {
		super (priority, testToRun, ti, rtv, tp);
		RTMBPriorityScheduler rtmbPriorityScheduler = new RTMBPriorityScheduler();
		PriorityParameters priorityParams = null;
		switch (priority) {
		case LowPriority:
			priorityParams = new PriorityParameters(rtmbPriorityScheduler.getMinPriority());
			break;
		case MedPriority:
			priorityParams = new PriorityParameters((rtmbPriorityScheduler.getMaxPriority() + 
					rtmbPriorityScheduler.getMinPriority()) / 2);
			break;
		case HighPriority:
			priorityParams = new PriorityParameters(rtmbPriorityScheduler.getMaxPriority());
		}
		rtThread = new RealtimeThread(priorityParams, null, null, null, null, this);
	}
}
