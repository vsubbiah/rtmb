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

import com.ibm.realtime.rtmb.tests.util.BenchMarkThread;
import com.ibm.realtime.rtmb.tests.util.PriorityType;
import com.ibm.realtime.rtmb.util.RTSJSpecificTest;
import com.ibm.realtime.rtmb.util.RTSJSpecificUtils.RTMBPriorityScheduler;

@RTSJSpecificTest
public class RTBenchMarkThread extends BenchMarkThread implements Runnable {
	private RealtimeThread rtThread;
	public RTBenchMarkThread (PriorityType priority, Runnable logic) {
		super();
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
    	rtThread = new RealtimeThread (priorityParams, null, null, null,null, logic);
	}

	/* (non-Javadoc)
	 * @see com.ibm.realtime.rtmb.tests.util.BenchMarkThread#execute()
	 */
	@Override
	public void execute() {
		rtThread.start();
	}

	/* (non-Javadoc)
	 * @see com.ibm.realtime.rtmb.tests.util.BenchMarkThread#waitForThreadExit()
	 */
	@Override
	public void waitForThreadExit() throws InterruptedException {
		rtThread.join();
	}

	@Override
	public void setThreadName(String name) {
		rtThread.setName(name);
	}
	
	/* (non-Javadoc)
	 * @see java.lang.Runnable#run()
	 */
	public void run() {
		/*
		 * Runnable target for the method was specified
		 * in the constructor. So, throw an error.
		 */
		throw new NoSuchMethodError("Thread found no Runnable targets");
	}
}
