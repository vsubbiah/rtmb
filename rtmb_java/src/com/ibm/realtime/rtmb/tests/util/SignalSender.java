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

public class SignalSender implements Runnable{
	BenchMarkThread thread  = null;
	int subIterations = 0;
	int iterations = 0;
	SignalReceiver rcvr = null;
	Object readyLock = null;
	Object waitNotifyLock = null;

	long diffTime[] = null;

	public SignalSender( ) {
	}

	public void setSignalReceiver( SignalReceiver rcvr ) {
		this.rcvr = rcvr;
	}
	
	public void start() {
		thread = BenchMarkThreadFactory.getInstance().getThread( PriorityType.HighPriority, this);
		try {
			thread.execute();
		} catch(Exception e) {
			e.printStackTrace();
		}
	}

	public long[] getTimeDifferenceValues() {
		return diffTime;
	}

	public void waitTillReceiverStarted() {
		synchronized(readyLock) {
			while( !rcvr.getStarted() ) {		
				try {	
					readyLock.wait();
				} catch(Exception e) {
					e.printStackTrace();

				}
			}	

            rcvr.setStarted(false);
		}
	}

	public void run() {
		long endTime  = 0 , startTime = 0 ,difference = 0;

		for( int i = 0 ; i < iterations; i++ ) {
			waitTillReceiverStarted();

			startTime = System.nanoTime();

            synchronized( waitNotifyLock ) {
                    for(int j = 0; j < subIterations ; j++ ) {
                            waitNotifyLock.notify();

                            try {
                                    waitNotifyLock.wait();
                            } catch(Exception e) {
                                    e.printStackTrace();
                            }
                    }
            }

			endTime = System.nanoTime();
			difference = endTime - startTime;

			diffTime[i] = difference;
		}			
	}

	public void join() throws InterruptedException {
		thread.waitForThreadExit();
	}

	public void setReadyLock(Object readyLock2) {
		readyLock = readyLock2;
	}

	public void setWaitNotifyLock(Object waitNotifyLock2) {
		waitNotifyLock = waitNotifyLock2;
	}

	public void setSubIterations(int subIterations2) {
		subIterations = subIterations2;
	}

	public void setIterations(int iterations2) {
        if(diffTime == null ) {
    		diffTime = new long[ iterations2 ];
        }    
		iterations = iterations2;
	}

}
