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

import com.ibm.realtime.rtmb.tests.impl.BenchMarkThreadFactory;

public class SignalReceiver implements Runnable {
	int iterations = 0;
	int subIterations = 0;

	Object readyLock = null;
	Object waitNotifyLock = null;
	volatile boolean isReceiverStarted = false;
	private BenchMarkThread benchMarkThread;
		
	public SignalReceiver( ) {
		isReceiverStarted = false;
	}

	public void setIterations(int iters) {
		iterations = iters;
	}

	public void setSubIterations(int subIters) {
		subIterations = subIters;
	}
	
	public void run() {
            for (int i = 0; i < iterations; i++) {
                    synchronized(waitNotifyLock) {
                            setReceiverStarted();

                            for (int j = 0; j < subIterations; j++) {
                                    try {
                                            // Wait for notification
                                            waitNotifyLock.wait();
                                    } catch (InterruptedException e) {
                                            e.printStackTrace();
                                    }

                                    waitNotifyLock.notify();
                            }
                    }
            }
	} // end of run

	private void setReceiverStarted() {
		synchronized(readyLock) {
			isReceiverStarted = true;
			readyLock.notify();
		}
	}

	public void start() {
		benchMarkThread = BenchMarkThreadFactory.getInstance().getThread( PriorityType.HighPriority, this);
		benchMarkThread.execute();
	}

	public void join() throws InterruptedException {
		benchMarkThread.waitForThreadExit();
	}

	public boolean getStarted() {
		return isReceiverStarted;
	}

	public void setStarted( boolean flag )  {
		isReceiverStarted=flag;
	}

	public void setReadyLock(Object readyLock2) {
		readyLock = readyLock2;
	}

	public void setWaitNotifyLock(Object waitNotifyLock2) {
		waitNotifyLock = waitNotifyLock2;
		
	}
}

