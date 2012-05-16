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

import javax.realtime.ImmortalMemory;
import javax.realtime.LTMemory;
import javax.realtime.NoHeapRealtimeThread;
import javax.realtime.PriorityParameters;
import javax.realtime.PriorityScheduler;
import javax.realtime.RealtimeThread;
import javax.realtime.ScopedMemory;

import com.ibm.realtime.rtmb.params.RealTimeValues;
import com.ibm.realtime.rtmb.params.TestInfo;
import com.ibm.realtime.rtmb.params.TestParams;
import com.ibm.realtime.rtmb.results.TestResults;
import com.ibm.realtime.rtmb.tests.ClockConfig;
import com.ibm.realtime.rtmb.tests.TimerConfig;
import com.ibm.realtime.rtmb.tests.util.Cpus;
import com.ibm.realtime.rtmb.tests.util.MatrixOperationsWithObjects;
import com.ibm.realtime.rtmb.tests.util.MatrixOperationsWithPrimitives;
import com.ibm.realtime.rtmb.tests.util.NotEligibleException;
import com.ibm.realtime.rtmb.tests.util.SpecTest;
import com.ibm.realtime.rtmb.tests.util.TestId;
import com.ibm.realtime.rtmb.util.RTSJSpecificTest;
import com.ibm.realtime.rtmb.util.VerbosePrint;

@RTSJSpecificTest
public class NHRTSupportTest extends SpecTest {
	int iterations;
	int counter;
	int subIters = 2;

	synchronized void setCounter(int threads) {
		counter = threads;
	}

	synchronized void decrementCounter() {
		counter--;
		if(counter == 0){
			notifyAll();
		}
	}

	synchronized void waitForThreads() {
		while(counter > 0){
			try {
				wait();
			} catch (InterruptedException e) {
			}
		}
	}

	@Override
	public TestResults[] benchMarkTest(TestInfo tInfo, RealTimeValues rtValues, 
			TestParams tParams) throws NotEligibleException {
		int nCpu = Cpus.onlineCpus;
		int count = 0;
		boolean retryMsg = false;
		TestResults testResults[] = null;
		double clockAccuracy = ClockConfig.getClockAccuracy();
		if(nCpu == 1) {
			throw new NotEligibleException ("NHRTTest: Skipped, this test is not " +
			"run on uniprocessor systems");
		}

		final int frequency = (int) TimerConfig.getWorkQuantum();

		/*
		 * Setup the number of iterations required for this test.
		 */
		if (tParams.isQuickRun()) {
			iterations = 1;
		} else {
			iterations = tInfo.getTestIterations() * rtValues.getMultiplier();
		}
		tInfo.setTestIterations(iterations);
		final int nThreads = nCpu/2;
		boolean retry = true;
		
		VerbosePrint.testStart(System.out, tParams, name(), tInfo.getTestIterations());

		while (retry) {
			retry = false;
			if (count < tInfo.getThreshold()) {
				if (retryMsg) {
					VerbosePrint.testRetry(System.out, tParams, name());
				}
				//create the realtime threads while we are on the heap
				PriorityParameters med = new PriorityParameters((PriorityScheduler.instance().getMaxPriority()+PriorityScheduler.instance().getMinPriority())/2);
				RT [] rts = new RT [nThreads];
				for( int i = 0 ; i < nThreads ; i++) {
					rts[i] = new RT(frequency, 5, med);
				}
				final long [][] results = new long[nThreads][iterations];
				final LTMemory scopes[] = new LTMemory[nThreads]; 
				final NHRT[] nhrts = new NHRT[nThreads];
				ImmortalMemory.instance().executeInArea(new Runnable(){
					public void run() {
						PriorityParameters high = new PriorityParameters(PriorityScheduler.instance().getMaxPriority());
						for (int j = 0 ; j < nThreads ; j++) {
							scopes[j] = new LTMemory(10 * 1024 * 1024);// reserve 10 mb
							nhrts[j] = new NHRT(results[j], frequency, iterations, 5, high, scopes[j], subIters);
						}
					}
				});
				testResults = new TestResults[nThreads];
				for (int i = 0; i < nThreads; i++) {
					testResults[i] = new TestResults(TestId.NHRT_SUPPORT_TEST, iterations, true);
					testResults[i].setTestResultName("Results for " + nhrts[i].getName());
				}

				setCounter(nThreads);
				for(int i = 0; i < nThreads ; i++) {
					rts[i].start();
					nhrts[i].start();
				}
				waitForThreads();
				for (int i = 0; i < nThreads; i++) {
					rts[i].done = true;
				}
				for (int i = 0; i < nThreads; i++) {
					for (int j = 0; j < results[i].length; j++) {
						testResults[i].setValuesAtNextIndex(results[i][j]);
					}
					if (testResults[i].getMin() > clockAccuracy) {
						count++;
						retryMsg = true;
						VerbosePrint.outPrintln (System.out, tParams, 1);
						VerbosePrint.streamPrintln (System.out, tParams, 1,
								"NHRTTest: " + testResults[i].getTestResultName() + ": ");
						if (tParams.isVerbose()) {
							if (tParams.getVerboseLevel() > 1) {
								double [] values = testResults[i].getValues();
								for (int j = 0; j < values.length; j++) {
									VerbosePrint.outPrint("NHRTTest: Time difference " +
											"between end and start time: ");
									VerbosePrint.outPrintf(System.out, tParams, 2, "%.3f us" + 
											VerbosePrint.getLineSeparator(), values[j]);
								}
							}
						}
						if(!testResults[i].checkStdDev(rtValues, tParams)) {
							retry = true;
						}
					} else {
						retry = true;
					}
				}
			} else {
				break;
			}
			for (int i = 0; i < nThreads; i++) {
				testResults[i].resetResults();
			}
			subIters *= 2;
		}
		return testResults;
	}

	static class RT extends RealtimeThread {
		private int frequency;
		private int workSize;
		private boolean done;

		public RT(int frequency, int workSize, PriorityParameters priority) {
			super(priority);
			this.frequency = frequency;
			this.workSize = workSize;
			this.done = false;
		}

		@Override
		public void run() {
			while (!done) {
				MatrixOperationsWithObjects mo  = new MatrixOperationsWithObjects(workSize);
				mo.matrixMultiplication();
				try {
					Thread.sleep(0, frequency);
				} catch (InterruptedException ie) {
				}
			}
		}
	}

	class NHRT extends NoHeapRealtimeThread {
		private int frequency;
		private int iterations;
		private long [] results;
		private MatrixOperationsWithPrimitives mo;
		private int subIters;

		public NHRT(long [] results, int frequency, int iterations, int workSize, 
				PriorityParameters priority, ScopedMemory mem, int subIters) {
			super(priority,mem);
			this.frequency = frequency;
			this.iterations = iterations;
			this.results = results;
			this.mo = new MatrixOperationsWithPrimitives(workSize);
			this.subIters = subIters;
		}

		@Override
		public void run() {
			final long [] thrResults = new long[iterations];
			for (int i = 0; i < iterations; i++) {
				long startTime = System.nanoTime();
				for (int j = 0; j < subIters; j++) {
					mo.matrixMultiplication();	
				}
				long endTime = System.nanoTime();
				thrResults[i] = endTime - startTime;
				try {
					Thread.sleep(0, frequency);
				} catch (InterruptedException ie) {
				}
			}
			RealtimeThread thr = new RealtimeThread() {
				@Override
				public void run() {
					System.arraycopy(thrResults, 0, results, 0, iterations);
					NHRTSupportTest.this.decrementCounter();
				}
			};
			thr.start();
			try {
				thr.join();
			} catch (InterruptedException e) {
				e.printStackTrace();
			}
		}

		public long[] getResults() {
			return results;
		}
	}
	
	@Override
	public String name() {
		return "NHRTSupport";
	}
		
}

