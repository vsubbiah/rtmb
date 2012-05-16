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
 
package com.ibm.realtime.rtmb.tests.RTSJSpecificTests;

import javax.realtime.AbsoluteTime;
import javax.realtime.AsyncEventHandler;
import javax.realtime.BoundAsyncEventHandler;
import javax.realtime.Clock;
import javax.realtime.PeriodicTimer;
import javax.realtime.PriorityParameters;
import javax.realtime.PriorityScheduler;
import javax.realtime.RelativeTime;

import com.ibm.realtime.rtmb.params.RealTimeValues;
import com.ibm.realtime.rtmb.params.TestInfo;
import com.ibm.realtime.rtmb.params.TestParams;
import com.ibm.realtime.rtmb.results.TestResults;
import com.ibm.realtime.rtmb.tests.ClockConfig;
import com.ibm.realtime.rtmb.tests.TimerConfig;
import com.ibm.realtime.rtmb.tests.util.SpecTest;
import com.ibm.realtime.rtmb.tests.util.TestId;
import com.ibm.realtime.rtmb.util.RTSJSpecificTest;
import com.ibm.realtime.rtmb.util.VerbosePrint;

@RTSJSpecificTest
public class PeriodicEventsTest extends SpecTest {
	TestParams tParams;

	@Override
	public TestResults[] benchMarkTest(TestInfo tInfo, RealTimeValues rtValues,	TestParams tParams) {
		int new_iters;

		/*
		 * Setup the number of iterations required for this test.
		 */
		if (tParams.isQuickRun()) {
			new_iters = 1;
		} else {
			new_iters = tInfo.getTestIterations() * rtValues.getMultiplier();
		}
		tInfo.setTestIterations(new_iters);
		this.tParams = tParams;
		/*
		 * Create the TestResult[] object.
		 */
		TestResults[] testResults = null;
		testResults = new TestResults[1];
		testResults[0] = new TestResults(TestId.PERIODIC_EVENTS_TEST, tInfo.getTestIterations(), true);
		testResults[0].setTestResultName("Periodic event test result");

		VerbosePrint.testStart(System.out, tParams, name(), tInfo.getTestIterations());

		PriorityParameters high = new PriorityParameters(PriorityScheduler.instance().getMaxPriority());

		for (int subIters = 1; subIters <= tInfo.getThreshold(); subIters++) {
			testResults[0].resetResults();
			testPeriodicEventsWRT(high, testResults[0], tInfo.getTestIterations(), subIters);
			if (checkResults(rtValues, tParams, testResults)) {
				break;
			}
			VerbosePrint.outPrintln (System.out, tParams, 1);
			VerbosePrint.streamPrintln (System.out, tParams, 1,
				"PeriodicEventsTest: Retrying test with a larger period to get a better variance...");	
		}
		return testResults;
	}

	private void testPeriodicEventsWRT(PriorityParameters params, TestResults testResults, 
			int iterations, int subIters) {
		AbsoluteTime startTime = Clock.getRealtimeClock().getTime();
		int timerAccuracy = (int) TimerConfig.getTimerAccuracy();
		startTime.add(2000, 0);
		RelativeTime period = new RelativeTime(0,timerAccuracy*subIters);
		Waiter waiter = new Waiter(1);
		AsyncEventHandler aeh = new MyHandler(iterations, startTime, period,testResults,waiter, tParams);
		aeh.setSchedulingParameters(params);
		PeriodicTimer timer = new PeriodicTimer(startTime,period,aeh);
		timer.start();
		waiter.waitForCompletion();
		timer.stop();
	}

	private boolean checkResults(RealTimeValues rtValues, TestParams testParams, TestResults[] results) {
		boolean rc = true;
		double clockAccuracy = ClockConfig.getClockAccuracy();
		for(int i = 0; i <results.length; i++) {
			double values [] = results[i].getValues();
			results[i].resetResults();
			for (int j = 0; j < values.length; j++) {
				if (values[j] < clockAccuracy) {
					results[i].setValuesAtNextIndex((long)clockAccuracy);
				} else {
					results[i].setValuesAtNextIndex(values[j]);
				}
			}
			VerbosePrint.outPrintln (System.out, testParams, 1);
			VerbosePrint.streamPrintln (System.out, testParams, 1, 
					"Test report for " + results[i].getTestResultName() + ":");
			if (!results[i].checkStdDev(rtValues, testParams)) {
				rc = false;
			}
		}
		return rc;
	}

	static class Waiter {
		int waitFor;
		public Waiter(int waitFor) {
			this.waitFor = waitFor;
		}

		public synchronized void waitForCompletion() {
			while(waitFor > 0) {
				try {
					wait();
				}catch (InterruptedException e) {
				}
			}
		}

		public synchronized void completed() { 
			if(--waitFor == 0) {
				notifyAll();
			}
		}
	}

	static class MyHandler extends BoundAsyncEventHandler {
		int fireCount = 0;
		int totalFires;
		long expectedStart;
		long period;
		TestResults results;
		Waiter waiter;
		TestParams tParams;

		public MyHandler(int totalFires, AbsoluteTime start, RelativeTime period, 
				TestResults results, Waiter waiter, TestParams tParams) {
			this.totalFires = totalFires;
			this.expectedStart = start.getMilliseconds()*1000000+start.getNanoseconds();
			this.period = period.getMilliseconds()*1000000+period.getNanoseconds();
			this.results = results;
			this.waiter = waiter;
			this.tParams = tParams;
		}

		@Override
		public void handleAsyncEvent() {
			if(fireCount < totalFires) {
				AbsoluteTime tm = Clock.getRealtimeClock().getTime();
				long currentTime = tm.getMilliseconds()*1000000 + tm.getNanoseconds();
				long diff = currentTime - expectedStart;
				VerbosePrint.streamPrintln(System.out, tParams, 2, 
						"PeriodicEventsTest: Difference between actual fire and " +
						"expected fire times: ");
				VerbosePrint.outPrintf(System.out, tParams, 2, "%.3f us" + 
						VerbosePrint.getLineSeparator(), diff);
				results.setValuesAtNextIndex(diff);
				fireCount++;
				expectedStart += period;
				if(fireCount == totalFires) {
					waiter.completed();
				}
			}
		}
	}

	@Override
	public String name() {
		return "PeriodicEvents";
	}	
}