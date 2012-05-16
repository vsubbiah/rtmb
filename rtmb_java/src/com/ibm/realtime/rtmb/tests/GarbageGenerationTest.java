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
import com.ibm.realtime.rtmb.tests.impl.BenchMarkThreadFactory;
import com.ibm.realtime.rtmb.tests.util.BenchMarkThread;
import com.ibm.realtime.rtmb.tests.util.Cpus;
import com.ibm.realtime.rtmb.tests.util.GCWorker;
import com.ibm.realtime.rtmb.tests.util.PriorityType;
import com.ibm.realtime.rtmb.tests.util.SpecTest;
import com.ibm.realtime.rtmb.tests.util.TestId;
import com.ibm.realtime.rtmb.util.VerbosePrint;

public class GarbageGenerationTest extends SpecTest {
	private static int counter = 0;
	private static Object lockObject = new Object();

	@Override
	public TestResults[] benchMarkTest(TestInfo tInfo, RealTimeValues rtValues, TestParams tParams) {
		int subIters = 100;
		GCWorker [] worker;
		int count = 0;
		double clockAccuracy = ClockConfig.getClockAccuracy();
		boolean retry = true;
		boolean retryMsg = false;
		int retryCounter  = 0;

		updateIterations(tInfo, rtValues, tParams);

		TestResults testResults[] = new TestResults[Cpus.onlineCpus];
		for (int i = 0; i < Cpus.onlineCpus; i++) {
			testResults[i] = new TestResults(TestId.GARBAGE_GENERATION_TEST,
					tInfo.getTestIterations(), true);
			testResults[i].setTestResultName("Thread " + i);
		}
		VerbosePrint.outPrintln(System.out, tParams, 1);
		VerbosePrint.streamPrintln(System.out, tParams, 1, 
		"Running GarbageGenerationTest Test...");
		VerbosePrint.streamPrintln(System.out, tParams, 1,
				"GarbageGenerationTest: This test may take a few minutes to " +
		"complete. Please be patient...");
		while (retry) {
			retry = false;
			retryCounter = 0;
			if (count >= tInfo.getThreshold()) {
				break;
			}
			if (retryMsg) {
				VerbosePrint.testRetry(System.out, tParams, name());
			}
			for (int i = 0; i < Cpus.onlineCpus; i++) {
				testResults[i].resetResults();
			}
			/*
			 * Create runnables for every worker thread
			 */
			worker = new GCWorker [Cpus.onlineCpus];
			for (int i = 0; i < Cpus.onlineCpus; i++) {
				worker[i] = new GCWorker(testResults[i], tInfo, tParams, subIters);
			}
			BenchMarkThreadFactory threadFactory = BenchMarkThreadFactory.getInstance();
			BenchMarkThread gcThreads[] = new BenchMarkThread[Cpus.onlineCpus];
			/*
			 * Start with no garbage on heap from previous tests and give GC 
			 * a fair playing ground.
			 */
			
			System.gc();
			int gcCounter = 0;
			for (int j = 0; j < Cpus.onlineCpus; gcCounter++, j++) {
				gcThreads[gcCounter]= threadFactory.getThread(PriorityType.HighPriority, worker[gcCounter]);
				gcThreads[gcCounter].setThreadName("High Priority GC test thread " + j);
				incrementCounter();
				/*
				 * Launch the worker threads to start doing the matrix 
				 * multiplication operation.
				 */
				gcThreads[gcCounter].execute();
			}
			waitForThreadsExit();
			for (int i = 0; i < Cpus.onlineCpus; i++) {
				if (testResults[i].getMedian() > clockAccuracy) {
					if (++retryCounter == (Cpus.onlineCpus)) {
						count++;
					}
					VerbosePrint.streamPrintln (System.out, tParams, 1,
							"Test result for " + testResults[i].getTestResultName());
					VerbosePrint.streamPrintln (System.out, tParams, 1,
					"==========================================");
					VerbosePrint.outPrintln (System.out, tParams, 1);
					if (!testResults[i].checkStdDev(rtValues, tParams)) {
						retryMsg = true;
						retry = true;
					}
				} else {
					retry = true;
				}
			}
			subIters *= 2;
		}
		return testResults;
	}

	public static void decrementCounter() {
		synchronized (lockObject) {
			assert (counter >= 0);
			if (counter > 0) {
				counter--;
				if (counter == 0) {
					lockObject.notify();
				}
			}
		}
	}

	private static synchronized void incrementCounter() {
		assert (counter >= 0);
		counter++;
	}

	private static void waitForThreadsExit() {
		synchronized (lockObject) {
			while (counter > 0) {
				try {
					lockObject.wait();
				} catch (InterruptedException e) {
					e.printStackTrace();
				}
			}

		}
	}
	
	@Override
	public String name() {
		return "GarbageGenerationTest";
	}
		
}

