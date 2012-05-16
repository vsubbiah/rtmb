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
 
/**
 * 
 */
package com.ibm.realtime.rtmb.tests.util;

public class StdBenchMarkThread extends BenchMarkThread implements Runnable {
	private Thread stdThread;
	
	public StdBenchMarkThread(PriorityType priority, Runnable logic) {
		super();
		stdThread = new Thread (logic);
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
	
	/* (non-Javadoc)
	 * @see com.ibm.realtime.rtmb.tests.util.BenchMarkThread#execute()
	 */
	@Override
	public void execute() {
		stdThread.start();

	}

	/* (non-Javadoc)
	 * @see com.ibm.realtime.rtmb.tests.util.BenchMarkThread#waitForThreadExit()
	 */
	@Override
	public void waitForThreadExit() throws InterruptedException {
		stdThread.join();
	}

	public void run() {
		throw new NoSuchMethodError ("Thread found no Runnable targets");
		
	}
	
	@Override
	public void setThreadName(String name) {
		stdThread.setName(name);
	}
}
