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
import com.ibm.realtime.rtmb.tests.util.DiskIOReadUtils;
import com.ibm.realtime.rtmb.tests.util.SpecTest;
import com.ibm.realtime.rtmb.tests.util.TestFinalDefinitions;
import com.ibm.realtime.rtmb.tests.util.TestId;
import com.ibm.realtime.rtmb.util.VerbosePrint;

public class DiskIOReadTest extends SpecTest {
	@Override
	public TestResults[] benchMarkTest(TestInfo tInfo, RealTimeValues rtValues,	TestParams tParams) {
		/* Other variables local to this function */
		int count = 0;
		int bufSize = TestFinalDefinitions.DISKIO_CONFIG_BSIZE;
		double clock = ClockConfig.getClockAccuracy(); 
		
		int new_iters;
		int loops = 1;
		DiskIOReadUtils readUtils = null;
		long startTime, endTime;
		boolean retryMsg = false;

		new_iters = updateIterations(tInfo, rtValues, tParams);
		TestResults[] testResult = establishSingleResult(TestId.DISKIOREAD_CONFIG, tInfo.getTestIterations());
		
		VerbosePrint.testStart(System.out, tParams, name(), tInfo.getTestIterations());		

		while ( true ) {
			if ( count >= tInfo.getThreshold() ) {
				break;
			}
			testResult[0].resetResults();

			if (retryMsg) {
				VerbosePrint.testRetry(System.out, tParams, name());
			}
			for (int loop = 0; loop < new_iters; loop++) {
				long timeDiff = 0;
				readUtils = new DiskIOReadUtils(bufSize);
				startTime = System.nanoTime();
				readUtils.doRead(loops);
				endTime = System.nanoTime();
				/* Get the elapsed time in nsecs */
				timeDiff = endTime - startTime;
				
				VerbosePrint.diffTime(System.out, tParams, name(), timeDiff);

				testResult[0].setValuesAtNextIndex(timeDiff);
				readUtils.cleanup();
			}
			if (testResult[0].getMedian() > clock) {
				retryMsg = true;
				count++;
				/* Check against the input parameters for this test */
				if(testResult[0].checkStdDev(rtValues, tParams)) {
					/* Variance looks good and acceptable */
					retryMsg = false;
					break;
				}
			}
			/* Variance is not good enough, so retry */
			loops *=  2;
		}
		return testResult;
	}
	
	@Override
	public String name() {
		return "DiskIORead";
	}
		
}