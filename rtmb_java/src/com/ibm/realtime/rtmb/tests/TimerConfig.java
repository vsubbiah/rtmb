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
import com.ibm.realtime.rtmb.tests.util.TestFinalDefinitions;
import com.ibm.realtime.rtmb.tests.util.TestId;
import com.ibm.realtime.rtmb.util.VerbosePrint;

public class TimerConfig extends SpecTest {
	private double max(double val1, double val2) {
		return (val1 > val2) ? val1 : val2;
	}

	private double min(double val1, double val2) {
		return (val1 < val2) ? val1 : val2;
	}

	public boolean runTimerTest(int num, TestResults result,
			TestParams tParams, int new_iters) {
		long startTime, endTime;
		long timeDiff = 0;
		int loop = 0;
		
		for (loop = 0; loop < new_iters; loop++) {
			startTime = System.nanoTime();
			try {
				Thread.sleep(0, num); // do some nano sec sleep
			} catch (InterruptedException e) {
				e.printStackTrace();
				return false;
			}
			endTime = System.nanoTime();

			/* Get the elapsed time in nsecs */
			timeDiff = endTime - startTime;
			//msf - a little too noisy - VerbosePrint.diffTime(System.out, tParams, name(), timeDiff);
			
			result.setValuesAtNextIndex(timeDiff);
		}

		return true;
	}

	@Override
	public TestResults[] benchMarkTest(TestInfo tInfo, RealTimeValues rtValues,
			TestParams tParams) {
		int count = 0;

		if (timerAccuracySpecified()) {
			VerbosePrint.streamPrint (System.out, tParams, 1, 
			"TimerConfig: Configuration test will not be run. Timer accuracy preconfigured as: ");
			VerbosePrint.streamPrintf(System.out, tParams, 1, "%.3f us" + 
					VerbosePrint.getLineSeparator(), 
					VerbosePrint.roundToMicroSec(getTimerAccuracy()));
			VerbosePrint.outPrintln (System.out, tParams, 1);
			
			return null;
		}
		

		
		double clock = ClockConfig.getClockAccuracy();
		int num = TestFinalDefinitions.TIMER_CONFIG_NSLEEP_TIME;
		int new_iters;

		boolean retryMsg = false;

		new_iters = updateIterations(tInfo, rtValues, tParams);
		TestResults[] testResult = establishSingleResult(TestId.SYSTIMER_CONFIG, tInfo.getTestIterations());

		VerbosePrint.testStart(System.out, tParams, name(), tInfo.getTestIterations());
		
		double savedAcceptablePercent = rtValues.getAcceptablePercent();
		double savedRangePercent = rtValues.getRangePercent();
		boolean flag = false;
		if (savedAcceptablePercent < 99.9 || savedRangePercent > 20.0) {
			flag = true;
		}
		
		double maximumPct = max(99.9, rtValues.getAcceptablePercent());
		rtValues.setAcceptablePercent(maximumPct);
		
		double minPct = min(20.0, rtValues.getRangePercent());
		rtValues.setRangePercent(minPct);
		if (flag) {
			VerbosePrint.outPrintln(System.out, tParams, 1);
			VerbosePrint.streamPrintln(System.out, tParams, 1,
					"TimerConfig: Altered the acceptable percentage to: "
							+ rtValues.getAcceptablePercent());
			VerbosePrint.streamPrintln(System.out, tParams, 1,
					"TimerConfig: Altered the range percentage to: "
							+ rtValues.getRangePercent());
			VerbosePrint.outPrintln(System.out, tParams, 1);
		}

		boolean testPass = false;
		int previousQuantum = 0;

		while (true) {
			boolean rc = false;
			if (count++ < tInfo.getThreshold()) {
				testResult[0].resetResults();

				rc = runTimerTest(num, testResult[0], tParams, new_iters);
				if (rc == false) {
					return null;
				}

				if (testResult[0].getMedian() > clock) {
					retryMsg = true;

					/* Check against the input parameters for this test */
					if (testResult[0].checkStdDev(rtValues, tParams)) {
						/* Variance looks good */
						retryMsg = false;
						testPass = true;
						break;
					}
				}
			} else {
				break;
			}

			if (retryMsg) {
				VerbosePrint.testRetry(System.out, tParams, name());
			}

			num = num * 2;
		}

		testResult[0].setWorkQuantum(num);

		if (testPass) {
			TestResults passedResult = null;
			int startQuantum = previousQuantum;
			int stopQuantum = num;
			int midQuantum = 0;

			passedResult = new TestResults(TestId.SYSTIMER_CONFIG, tInfo
					.getTestIterations(), true);
			passedResult.cloneValues(testResult[0]);

			TestResults tResult = new TestResults(TestId.SYSTIMER_CONFIG, tInfo
					.getTestIterations(), true);

			while (startQuantum <= stopQuantum) {
				tResult.resetResults();
				midQuantum = (startQuantum + stopQuantum) / 2;

				VerbosePrint.streamPrintln(System.out, tParams, 1,
						"TimerConfig: Refine Work Quantum. Range from: " + startQuantum + " to: " + stopQuantum);

				runTimerTest(midQuantum, tResult, tParams, new_iters);

				if (tResult.checkStdDev(rtValues, tParams)) {
					passedResult.resetResults();
					tResult.setWorkQuantum(midQuantum);
					passedResult.cloneValues(tResult);
					stopQuantum = midQuantum - 1;
				} else {
					startQuantum = midQuantum + 1;
				}
			}

			testResult[0] = passedResult;
		}

		VerbosePrint.streamPrintln(System.out, tParams, 1,
				"TimerConfig: WorkQuantum = " + testResult[0].getWorkQuantum());

		VerbosePrint.streamPrint(System.out, tParams, 1,
				"TimerConfig: Benchmark will be configured "
						+ "to expect the timer accuracy to be accurate to ");
		VerbosePrint.streamPrintf(System.out, tParams, 1, "%.3f us"
				+ VerbosePrint.getLineSeparator(), 
				VerbosePrint.roundToMicroSec(testResult[0].getMedian()));
		VerbosePrint.outPrintln(System.out, tParams, 1);

		if (flag) {
			rtValues.setAcceptablePercent(savedAcceptablePercent);
			rtValues.setRangePercent(savedRangePercent);
		}

		return testResult;
	}

	/**
	 * 
	 * @return The accuracy of the timer, which is currently given by the median
	 *         result returned from running the timer configuration test. 
	 */
	private static double accuracy = 0.0;
	public static double getTimerAccuracy() {
		if (accuracy == 0.0) {
			accuracy = (ResultTable.getInstance().getTestResultsByTestID(TestId.SYSTIMER_CONFIG)[0].getMedian());
			System.out.println("TimerConfig Accuracy: " + accuracy);
		}
		return accuracy;
	}

	/**
	 * getWorkQuantum - msf - need something less vague here
	 * @return a work quantum, based on the timer accuracy, that is large enough to represent reasonable work
	 */
	public static double getWorkQuantum() {
		return getTimerAccuracy();
		/*
		if (quantum == 0.0) {
			quantum = (ResultTable.getInstance().getTestResultsByTestID(TestId.SYSTIMER_CONFIG)[0].getWorkQuantum());
			System.out.println("TimerConfig Accuracy: " + getTimerAccuracy());
			System.out.println("TimerConfig Quantum: " + quantum);
		}
		return quantum;
		*/
	}	

	/**
	 * Explicitly set the timer accuracy from a command-line option
	 * 
	 * @param time : String representing the timer accuracy in the form <xx>[ms|us|ns], e.g. 10us
	 * @return : true if the timer was not set (because the format is invalid), false otherwise
	 */
	public static boolean setTimerAccuracy(String time) {
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
	
	public static boolean timerAccuracySpecified() {
		return accuracy != 0.0;
	}
	
	
	@Override
	public String name() {
		return "TimerConfig";
	}
}