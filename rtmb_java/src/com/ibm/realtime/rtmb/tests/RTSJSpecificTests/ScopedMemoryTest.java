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
import javax.realtime.ScopedMemory;

import com.ibm.realtime.rtmb.params.RealTimeValues;
import com.ibm.realtime.rtmb.params.TestInfo;
import com.ibm.realtime.rtmb.params.TestParams;
import com.ibm.realtime.rtmb.results.TestResults;
import com.ibm.realtime.rtmb.results.TestScores;
import com.ibm.realtime.rtmb.tests.ClockConfig;
import com.ibm.realtime.rtmb.tests.TimerConfig;
import com.ibm.realtime.rtmb.tests.util.FinalObject;
import com.ibm.realtime.rtmb.tests.util.MathOperations;
import com.ibm.realtime.rtmb.tests.util.ReferObject;
import com.ibm.realtime.rtmb.tests.util.SpecTest;
import com.ibm.realtime.rtmb.tests.util.TestId;
import com.ibm.realtime.rtmb.util.RTSJSpecificTest;
import com.ibm.realtime.rtmb.util.VerbosePrint;

@RTSJSpecificTest
public class ScopedMemoryTest extends SpecTest {
	LTMemory[] scopeChain = null;
	String [] subTests = {"referLocal", "referNested - depth 1", "referNested - depth 2", 
			"referNested - depth 3", "referNested - depth 4", "referPortal", "enterAndExitScopes",
	"enterAndExitScopesWithFinalization"};
	TestInfo tInfo = null;
	RealTimeValues rtValues = null;

	@Override
	public TestResults[] benchMarkTest(TestInfo tInfo, RealTimeValues rtValues,	TestParams tParams) {
		int new_iters;
		this.rtValues = rtValues;
		this.tInfo = tInfo;

		/*
		 * Setup the number of iterations required for this test.
		 */
		if (tParams.isQuickRun()) {
			new_iters = 1;
		} else {
			new_iters = tInfo.getTestIterations() * rtValues.getMultiplier();
		}
		tInfo.setTestIterations(new_iters);

		/*
		 * Create the TestResult[] object. This test generates
		 * eight different scores and hence that many results. 
		 * So, the TestResult[] array will just have 8 elements 
		 * in it.
		 */
		TestResults[] testResult = new TestResults[8];
		for(int i = 0; i < 8 ; i++) {
			testResult[i] = new TestResults(TestId.SCOPE_ACCESS_TEST, 
					tInfo.getTestIterations(), true);
		}
		
		VerbosePrint.testStart(System.out, tParams, name(), tInfo.getTestIterations());		

		//run all the 8 sub-tests;
		referLocal  (testResult[0], new_iters, tParams, new ScopedMemoryBoolean());
		referNested (testResult[1], new_iters, tParams, 1);
		referNested (testResult[2], new_iters, tParams, 2);
		referNested (testResult[3], new_iters, tParams, 3);
		referNested (testResult[4], new_iters, tParams, 4);
		referPortal (testResult[5], new_iters, tParams, new ScopedMemoryBoolean());
		enterExitScopes (testResult[6], new_iters, tParams, new ScopedMemoryBoolean());
		enterExitScopesWithFinalization (testResult[7], new_iters, tParams, new ScopedMemoryBoolean());
		return testResult;
	}

	@Override
	public TestScores [] generateCombinedScore (TestInfo tInfo, TestResults [] tResult) {
		TestResults [] tmpTestResults = new TestResults[1];
		TestScores [] ScopedMemoryTestScore = new TestScores[tResult.length];
		TestScores [] dummyTestScore;
		for (int i = 0; i < tResult.length; i++) {
			tmpTestResults[0] = tResult[i];
			dummyTestScore = super.generateCombinedScore(tInfo, tmpTestResults);
			ScopedMemoryTestScore[i] = dummyTestScore[0];
			ScopedMemoryTestScore[i].setTestScoreName(subTests[i]);
		}
		return ScopedMemoryTestScore;
	}

	private void referLocal(final TestResults results, final int iterations, final TestParams tParams, final ScopedMemoryBoolean tStatus) {
		results.setTestResultName("referLocal");
		int count = 0;
		boolean retryMsg = false;
		final LTMemory mem = new LTMemory(10240);
		VerbosePrint.outPrintln(System.out, tParams, 1);
		VerbosePrint.streamPrintln(System.out, tParams, 1,
				"ScopedMemoryTest: Test result for subtest " + 
				results.getTestResultName() + ":");
		while (!tStatus.isDone()) {
			if (count < tInfo.getThreshold()) {
				if (retryMsg) {
					VerbosePrint.testRetry(System.out, tParams, name());
				}
				for(int i = 0; i < iterations; i++) {
					mem.enter(new Runnable() {
						public void run() {
							long timeDiff;
							ReferObject localObject = new ReferObject();
							long startTime = System.nanoTime();
							for (int j = 0; j < tStatus.getSubIters(); j++) {
								localObject.setValue(100);
								localObject.getValue();
							}
							long endTime = System.nanoTime();
							// Get the elapsed time in nsecs
							timeDiff = endTime - startTime;
							VerbosePrint.streamPrintln(System.out, 
									tParams, 2, "ScopeTest - Local access: Difference between " +
									"end and start times = " + timeDiff + " ns");
							results.setValuesAtNextIndex(timeDiff);
						}
					});
				}
				if (results.getMedian() > ClockConfig.getClockAccuracy()) {
					retryMsg = true;
					count++;
					if (results.checkStdDev(rtValues, tParams)) {
						retryMsg = false;
						tStatus.setDone(true);
						tStatus.setSubIters(1);
					}
				}
			} else {
				break;
			}
			tStatus.setSubIters(tStatus.getSubIters() * 2);
			results.resetResults();
		}
	}

	private void referNested(final TestResults results, final int iterations, 
			final TestParams tParams, final int depth) {
		results.setTestResultName("referNested with depth " + depth);

		final int quantum = (int) TimerConfig.getWorkQuantum();
		VerbosePrint.outPrintln(System.out, tParams, 1);
		VerbosePrint.streamPrintln(System.out, tParams, 1,
				"ScopedMemoryTest: Test result for subtest " + 
				results.getTestResultName() + ":");
		ImmortalMemory im = ImmortalMemory.instance();
		im.enter(new Runnable () {
			public void run() {
				scopeChain = new LTMemory[depth+1];
			}
		});
		for(int i = 0; i < scopeChain.length; i++) {
			scopeChain[i] = new LTMemory(1024 * 1024 * 10);
		}
		scopeChain[0].enter(new Runnable() {
			public void run() {
				ScopeTestRunnable s = new ScopeTestRunnable(scopeChain, 1, null, results, iterations, quantum, tParams);
				s.run();
			}
		});
	}

	private void referPortal(final TestResults results, final int iterations, final TestParams tParams, final ScopedMemoryBoolean testStatus) {
		int count = 0;
		boolean retryMsg = false;
		results.setTestResultName("referPortal");
		final LTMemory mem = new LTMemory(10240);
		VerbosePrint.outPrintln(System.out, tParams, 1);
		VerbosePrint.streamPrintln(System.out, tParams, 1,
				"ScopedMemoryTest: Test result for subtest " + 
				results.getTestResultName() + ":");
		while (!testStatus.isDone()) {
			if (count < tInfo.getThreshold()) {
				if (retryMsg) {
					VerbosePrint.testRetry(System.out, tParams, name());
				}
				for (int i = 0; i < iterations; i++) {
					mem.enter(new Runnable() {
						public void run() {
							long timeDiff;
							ReferObject portal = new ReferObject();
							mem.setPortal(portal);
							long startTime = System.nanoTime();
							for (int j = 0; j < testStatus.getSubIters(); j++) {
								ReferObject p = (ReferObject)mem.getPortal();
								p.setValue(100);
								p.getValue();
							}
							long endTime = System.nanoTime();
							// Get the elapsed time in nsecs
							timeDiff = endTime - startTime;
							VerbosePrint.streamPrintln(System.out, 
									tParams, 2, "ScopeTest - Portal access: Difference between " +
									"end and start times = " + timeDiff + " ns");
							results.setValuesAtNextIndex(timeDiff);
						}
					});
				}
				if (results.getMedian() > ClockConfig.getClockAccuracy()) {
					retryMsg = true;
					count++;
					if (results.checkStdDev(rtValues, tParams)) {
						testStatus.setDone(true);
						retryMsg = false;
						testStatus.setSubIters(1);
					}
				}
			} else {
				break;
			}
			testStatus.setSubIters(testStatus.getSubIters() * 2);
			results.resetResults();
			}
		}

		private void enterExitScopes(final TestResults results, final int iterations, final TestParams tParams, final ScopedMemoryBoolean tStatus) {
			int count = 0;
			results.setTestResultName("enterExitScopes");
			final LTMemory mem = new LTMemory(10240);
			long timeDiff;
			boolean retryMsg = false;
			VerbosePrint.outPrintln(System.out, tParams, 1);
			VerbosePrint.streamPrintln(System.out, tParams, 1,
					"ScopedMemoryTest: Test result for subtest " + 
					results.getTestResultName() + ":");
			while (!tStatus.isDone()) {
				if (count < tInfo.getThreshold()) {
					if (retryMsg) {
						VerbosePrint.testRetry(System.out, tParams, name() + "(enterExitScopes)");
					}
					for (int i = 0; i < iterations; i++) {
						long startTime = System.nanoTime();
						for (int j = 0; j < tStatus.getSubIters(); j++) {
							mem.enter(new Runnable(){
								public void run() {
									MathOperations.addNumbers(500);
								}
							});
						}
						long endTime = System.nanoTime();
						// Get the elapsed time in nsecs
						timeDiff = endTime - startTime;
						VerbosePrint.streamPrintln(System.out, 
								tParams, 2, "ScopeTest - Enter-Exit: Difference between " +
								"end and start times = " + timeDiff + " ns");
						results.setValuesAtNextIndex(timeDiff);
					}
					if (results.getMedian() > ClockConfig.getClockAccuracy()) {
						retryMsg = true;
						count++;
						if (results.checkStdDev(rtValues, tParams)) {
							tStatus.setDone(true);
							tStatus.setSubIters(1);
							retryMsg = false;
						}
					}
				} else {
					break;
				}
				tStatus.setSubIters(tStatus.getSubIters() * 2);
				results.resetResults();
			}
		}

		private void enterExitScopesWithFinalization(final TestResults results, 
				final int iterations, final TestParams tParams, final ScopedMemoryBoolean tStatus) {
			int count = 0;
			results.setTestResultName("enterExitScopesWithFinalization");
			final LTMemory mem = new LTMemory(10240);
			long timeDiff;
			boolean retryMsg = false;
			VerbosePrint.outPrintln(System.out, tParams, 1);
			VerbosePrint.streamPrintln(System.out, tParams, 1,
					"ScopedMemoryTest: Test result for subtest " + 
					results.getTestResultName() + ":");
			while (!tStatus.isDone()) {
				if (count < tInfo.getThreshold()) {
					if (retryMsg) {
						VerbosePrint.testRetry(System.out, tParams, name() + "(enterExitScopesWithFinalization)");
					}
					for(int i = 0; i < iterations ;i++) {
						long startTime = System.nanoTime();
						for (int j = 0; j < tStatus.getSubIters(); j++) {
							mem.enter(new Runnable(){
								public void run(){
									FinalObject []objects = new FinalObject[10];
									for(int i = 0; i < 10;i++) {
										objects[i] = new FinalObject();
									}
								}
							});
						}
						long endTime = System.nanoTime();
						// Get the elapsed time in nsecs
						timeDiff = endTime - startTime;
						VerbosePrint.streamPrintln(System.out, 
								tParams, 2, "ScopeTest - Enter-Exit-With-Finalization: " +
								"Difference between end and start times = " + 
								timeDiff + " ns");
						results.setValuesAtNextIndex(timeDiff);
					}
					if (results.getMedian() > ClockConfig.getClockAccuracy()) {
						retryMsg = true;
						count++;
						if (results.checkStdDev(rtValues, tParams)) {
							tStatus.setSubIters(1);
							retryMsg = false;
							tStatus.setDone(true);
						}
					}
				} else {
					break;
				}
				tStatus.setSubIters(tStatus.getSubIters() * 2);
				results.resetResults();
			}
		}

		class ScopeTestRunnable implements Runnable {
			ScopedMemory [] scopeChain;
			int startIndex;
			ReferObject referObject;
			TestResults results;
			int iterations;
			int quantum;
			TestParams tParams;
			int subIters;

			public ScopeTestRunnable(final ScopedMemory [] scopeChain, final int startIndex, 
					final ReferObject referObject, final TestResults results, int iterations, 
					int quantum, TestParams tParams){
				this.scopeChain = scopeChain;
				this.startIndex = startIndex;
				this.referObject = referObject;
				this.results = results;
				this.iterations = iterations;
				this.quantum = quantum;
				this.tParams = tParams;
				this.subIters = 1;
			}

			public void run() {
				int count = 0;
				boolean retryMsg = false;
				if (referObject == null) {
					referObject = new ReferObject();
				}
				if (startIndex == scopeChain.length - 1) {
					while (true) {
						if (count < tInfo.getThreshold()) {
							if (retryMsg) {
								VerbosePrint.testRetry(System.out, tParams, name() + "(ReferNested)");
							}
							for(int i = 0; i < iterations; i++) {
								scopeChain[startIndex].enter(new Runnable(){
									public void run(){
										long startTime = System.nanoTime();
										for (int j = 0; j < subIters; j++) {
											referObject.setValue(100);
											referObject.getValue();
										}
										long endTime = System.nanoTime();
										long timeDiff = endTime - startTime;
										VerbosePrint.streamPrintln(System.out, 
												tParams, 2, "ScopeTest - Nested-Scope - Depth - " + 
												startIndex + ": Difference between end and start " +
												"times = " + timeDiff + " ns");
										results.setValuesAtNextIndex(timeDiff);
									}
								});
							}
							if (results.getMedian() > ClockConfig.getClockAccuracy()) {
								count++;
								retryMsg = true;
								if (results.checkStdDev(rtValues, tParams)) {
									retryMsg = false;
									break;
								}
							}
						} else {
							break;
						}
						subIters*= 2;
						results.resetResults();
					}
				} else {
					scopeChain[startIndex].enter(new ScopeTestRunnable(scopeChain, startIndex+1,
							referObject, results,iterations, quantum, tParams));
				}
			}
		}

		@Override
		public String name() {
			return "ScopedMemory";
		}	

	}

	class ScopedMemoryBoolean {
		private boolean done;
		private int subIters;
		public ScopedMemoryBoolean() {
			this.done = false;
			this.subIters = 1;
		}
		public boolean isDone() {
			return done;
		}
		public void setDone(boolean done) {
			this.done = done;
		}
		public int getSubIters() {
			return subIters;
		}
		public void setSubIters(int subIters) {
			this.subIters = subIters;
		}
	}