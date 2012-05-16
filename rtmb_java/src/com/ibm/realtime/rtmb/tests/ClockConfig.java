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
import com.ibm.realtime.rtmb.results.ResultTable;
import com.ibm.realtime.rtmb.results.TestResults;
import com.ibm.realtime.rtmb.tests.util.SpecTest;
import com.ibm.realtime.rtmb.tests.util.TestId;
import com.ibm.realtime.rtmb.util.VerbosePrint;

public class ClockConfig extends SpecTest {
	@Override
	public TestResults[] benchMarkTest(TestInfo tInfo, RealTimeValues rtValues, TestParams tParams) {
		int new_iters;
		long startTime, endTime;

		if (clockAccuracySpecified()) {
			VerbosePrint.streamPrint (System.out, tParams, 1, 
			"ClockConfig: Configuration test will not be run. Latency of clock lookup procedures preconfigured as: ");
			VerbosePrint.streamPrintf(System.out, tParams, 1, "%.3f us" + 
					VerbosePrint.getLineSeparator(), 
					VerbosePrint.roundToMicroSec(getClockAccuracy()));
			VerbosePrint.outPrintln (System.out, tParams, 1);
			
			return null;
		}

		VerbosePrint.testStart(System.out, tParams, name(), tInfo.getTestIterations());	
		new_iters = updateIterations(tInfo, rtValues, tParams);
		TestResults[] testResult = establishSingleResult(TestId.SYSCLOCK_CONFIG, tInfo.getTestIterations());
		
		for (int loop = 0; loop < new_iters; loop++) {
			long timeDiff;
			startTime = System.nanoTime();
			endTime = System.nanoTime();
			
			/* Get the elapsed time in nsecs */
			timeDiff = endTime - startTime;
			if (timeDiff <= 0) {
				/**
				 * time difference was 0 (or negative) which means that
				 * the call managed to probably read the old
				 * clock value. Round this up to 1ns to avoid spurious
				 * results later with infinite throughput and the like.
				 */
				timeDiff = 1;
			}
			//msf - a little too verbose VerbosePrint.diffTime(System.out, tParams, name(), timeDiff);					

			testResult[0].setValuesAtNextIndex(timeDiff);
		}
		
		VerbosePrint.streamPrint (System.out, tParams, 1, 
				"ClockConfig: Benchmark is configured to expect latency of clock lookup procedures to be ");
		VerbosePrint.streamPrintf(System.out, tParams, 1, "%.3f us" + 
				VerbosePrint.getLineSeparator(), 
				VerbosePrint.roundToMicroSec(testResult[0].getMedian()));
		VerbosePrint.outPrintln (System.out, tParams, 1);

		return testResult;
	}

	/**
	 * 
	 * @return The accuracy of the clock, which is currently given by the heuristic of 20 times the median
	 *         result returned from running the clock configuration test. 
	 */
	private static double accuracy = 0.0;
	public static double getClockAccuracy() {
		if (accuracy == 0.0) {
			accuracy = (ResultTable.getInstance().getTestResultsByTestID(TestId.SYSCLOCK_CONFIG)[0].getMedian());
		}
		return 20*accuracy;
	}

	@Override
	public String name() {
		return "ClockConfig";
	}

	/**
	 * Explicitly set the clock accuracy from a command-line option
	 * 
	 * @param time : String representing the clock accuracy in the form <xx>[ms|us|ns], e.g. 10us
	 * @return : true if the clock was not set (because the format is invalid), false otherwise
	 */
	public static boolean setClockAccuracy(String time) {
		long multiplier;
		if (time.endsWith("ns")) {
			multiplier = 1L;
		} else if (time.endsWith("us")) {
			multiplier = 1000L;
		} else if (time.endsWith("ms")) {
			multiplier = 1000000L;
		} else {
			return true;
		}
		long value = Long.parseLong(time.substring(0,time.length()-2).trim());
		if (value <= 0L) {
			return true;
		}
		accuracy = (double) (value * multiplier);
		return false;
	}
	
	public static boolean clockAccuracySpecified() {
		return accuracy != 0.0;
	}
	
}
