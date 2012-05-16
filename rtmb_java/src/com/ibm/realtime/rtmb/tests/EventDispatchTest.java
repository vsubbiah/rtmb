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
 
package com.ibm.realtime.rtmb.tests;

import com.ibm.realtime.rtmb.params.RealTimeValues;
import com.ibm.realtime.rtmb.params.TestInfo;
import com.ibm.realtime.rtmb.params.TestParams;
import com.ibm.realtime.rtmb.results.TestResults;
import com.ibm.realtime.rtmb.tests.util.SignalReceiver;
import com.ibm.realtime.rtmb.tests.util.SignalSender;
import com.ibm.realtime.rtmb.tests.util.SpecTest;
import com.ibm.realtime.rtmb.tests.util.TestId;
import com.ibm.realtime.rtmb.util.VerbosePrint;


public class EventDispatchTest extends SpecTest {
	SignalSender sender = null;
	SignalReceiver rcvr = null;
	private int iterations;
	private int subIterations = 2;

	@Override
	public TestResults[] benchMarkTest(TestInfo tInfo, RealTimeValues rtValues, TestParams tParams)  {
		/* Other variables local to this function */
		int new_iters;
		int count = 0;
		sender = new SignalSender();
		rcvr = new SignalReceiver();

		new_iters = updateIterations(tInfo, rtValues, tParams);
		TestResults[] testResult = establishSingleResult(TestId.EVENT_DISPATCH_TEST, tInfo.getTestIterations());
		iterations = new_iters;		

		VerbosePrint.streamPrintln (System.out, tParams, 2,
				"EventDispatchTest: Total number of iterations: " +
				tInfo.getTestIterations());

		VerbosePrint.testStart(System.out, tParams, name(), tInfo.getTestIterations());
		
		while ( true ) {
			testResult[0].resetResults();
			/* Get the elapsed time in nsecs */
			doRun();
			long diffTime[] = sender.getTimeDifferenceValues();
			for(int i = 0 ; i < iterations; i++ ) {
				testResult[0].setValuesAtNextIndex(diffTime[i]);
				VerbosePrint.diffTime(System.out, tParams, name(), diffTime[i]);				
			}	

			if( testResult[0].getMedian() < ClockConfig.getClockAccuracy() ) {
				subIterations *= 2;
			} else {
				if (testResult[0].checkStdDev(rtValues, tParams)) {
					break;
				} else {
					subIterations*= 2;

					/* If we have completed, throw exception */
					if ( ++count == tInfo.getThreshold() ) {
						break;
					}
					VerbosePrint.testRetry(System.out, tParams, name());
				}
			}
		}
		return testResult;
	}

	public void doRun( ) {
		Object readyLock = new Object();
		Object waitNotifyLock = new Object();

		sender.setReadyLock(readyLock);
		sender.setWaitNotifyLock(waitNotifyLock);
		sender.setIterations(iterations);
		sender.setSubIterations(subIterations);
		sender.setSignalReceiver(rcvr);

		rcvr.setReadyLock(readyLock);
		rcvr.setWaitNotifyLock(waitNotifyLock);
		rcvr.setSubIterations(subIterations);
		rcvr.setIterations(iterations);

		rcvr.start();
		sender.start();

		try {
			sender.join();
			rcvr.join();
		} catch(Exception e) {
			e.printStackTrace();
		}	
	}
	
	@Override
	public String name() {
		return "EventDispatchTest";
	}
		
}

