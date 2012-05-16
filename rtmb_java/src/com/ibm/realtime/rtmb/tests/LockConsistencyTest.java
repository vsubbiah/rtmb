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
import com.ibm.realtime.rtmb.tests.util.NumberWrapper;
import com.ibm.realtime.rtmb.tests.util.SpecTest;
import com.ibm.realtime.rtmb.tests.util.TestId;
import com.ibm.realtime.rtmb.util.VerbosePrint;

public class LockConsistencyTest extends SpecTest {
	TestResults lockResult;
	double medianTimeForClockAccuracyTest=0;

	TestParams tParams = null;
	RealTimeValues rtValues = null;
	TestInfo tInfo = null;

	public void runTest (TestResults lockResult) {
		long lockAcquireStart = 0;
		long lockReleaseEnd = 0;
		int subIters = 100;
		int count = 0;
		NumberWrapper num = new NumberWrapper();
		boolean retryMsg = false;

		while( true ) {
			if (count >= tInfo.getThreshold()) {
				break;
			}
			lockResult.resetResults();
			if (retryMsg) {
				VerbosePrint.testRetry(System.out, tParams, name());
			}
			Object lock = new Object();
			for( int i = 0 ; i < tInfo.getTestIterations(); i++ ) {
				lockAcquireStart  = System.nanoTime();
				for( int j = 0 ; j < subIters ; j++ ) {
					synchronized (lock) {
						num.setNumber(j);
					}
				}
				lockReleaseEnd  = System.nanoTime();
				// find the time to lock and unlock  + sleep
				long timeForIteration = lockReleaseEnd - lockAcquireStart;

				VerbosePrint.diffTime(System.out, tParams, name(), timeForIteration);					

				// add to resultTable
				lockResult.setValuesAtNextIndex(timeForIteration);
			}
			if ( lockResult.getMedian() >  medianTimeForClockAccuracyTest ) {
				retryMsg = true;
				count++;
				if (lockResult.checkStdDev(rtValues, tParams)) {
					retryMsg = false;
					break;
				}
			}
			subIters *= 2;
		}
	}

	@Override
	public TestResults[] benchMarkTest(TestInfo tInfo, RealTimeValues rtValues, TestParams tParams) {
		this.tParams = tParams;
		this.rtValues = rtValues;
		this.tInfo = tInfo;

		/* Other variables local to this function */
		int new_iters;

		new_iters = updateIterations(tInfo, rtValues, tParams);

		VerbosePrint.testStart(System.out, tParams, name(), tInfo.getTestIterations());
		
		lockResult = new TestResults(TestId.LOCK_CONSISTENCY_TEST, new_iters, true);
		medianTimeForClockAccuracyTest = ClockConfig.getClockAccuracy();	
		runTest(lockResult);
		TestResults[] retVal = new TestResults[1];
		lockResult.setTestResultName("Lock/unlock Test for High Priority Thread");
		retVal[0] = lockResult;
		return retVal;
	}
	
	@Override
	public String name() {
		return "LockConsistencyTest";
	}
		
}

