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
import com.ibm.realtime.rtmb.tests.util.SpecTest;
import com.ibm.realtime.rtmb.tests.util.TestId;
import com.ibm.realtime.rtmb.util.VerbosePrint;

public class TimeAccuracyTest extends SpecTest {
	@Override
	public TestResults[] benchMarkTest(TestInfo tInfo, RealTimeValues rtValues, TestParams tParams) {
		int new_iters;

		new_iters = updateIterations(tInfo, rtValues, tParams);
		TestResults[] testResult = establishSingleResult(TestId.TIME_ACCURACY_TEST, tInfo.getTestIterations());

		int sleepTimeNanos = (int)TimerConfig.getWorkQuantum();
		int newSleepTimeNanos = sleepTimeNanos;
		long sleepTimeMillis = 0;

		VerbosePrint.testStart(System.out, tParams, name(), tInfo.getTestIterations());
		
		if (sleepTimeNanos > 999999) {
			sleepTimeMillis = sleepTimeNanos / 1000000;
			sleepTimeNanos %= 1000000;
		}
		for (int loop = 0; loop < new_iters; loop++) {
			long startTime, endTime;
			long timeDiff = 0;
			startTime = System.nanoTime();
			try {
				// Do some nano sec sleep
				Thread.sleep(sleepTimeMillis, sleepTimeNanos); 
			} catch (InterruptedException e) {
				e.printStackTrace();
				return null;
			}
			endTime = System.nanoTime();
			/* Get the elapsed time in nsecs */
			timeDiff = endTime - startTime;
			testResult[0].setValuesAtNextIndex(timeDiff - newSleepTimeNanos);
			VerbosePrint.streamPrint (System.out, 
					tParams, 2, "TimeAccuracyTest: Difference between " +
			"actual and requested sleep time is: ");
			VerbosePrint.outPrintf(System.out, tParams, 2, "%.3f us" + 
					VerbosePrint.getLineSeparator(), (timeDiff - newSleepTimeNanos));
		}

		if (testResult[0].checkStdDev(rtValues, tParams)) {
			VerbosePrint.outPrintln(System.out, tParams, 1);
			VerbosePrint.streamPrintln(System.out, tParams, 1, 
					"TimeAccuracyTest: High resolution timer and clock are " +
			"consistent relative to one another.");
			VerbosePrint.outPrintln(System.out, tParams, 1);
		}
		return testResult;
	}

	@Override
	public String name() {
		return "TimeAccuracyTest";
	}	

}
