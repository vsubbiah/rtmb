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
import com.ibm.realtime.rtmb.results.TestScores;
import com.ibm.realtime.rtmb.tests.util.NotEligibleException;
import com.ibm.realtime.rtmb.tests.util.PriorityType;
import com.ibm.realtime.rtmb.tests.util.SpecTest;
import com.ibm.realtime.rtmb.tests.util.SyncCounter;
import com.ibm.realtime.rtmb.tests.util.TestId;
import com.ibm.realtime.rtmb.tests.util.WorkerThread;
import com.ibm.realtime.rtmb.util.VerbosePrint;

/** 
 * This thread class contains the code to run thread priority tests.
 * This class will create High ,medium and low priority threads simultaneously .
 * Based on the time taken for completion of tasks determines whether the threads have completed  
 * according to priority.
 */
public class ThreadPriorityTest extends SpecTest{
	
	// Size of matrix that needs to be multiplied as part of this test
	private static final int MATRIX_SIZE = 5;
	
	// array of worker threads 
	private WorkerThread t[];

	// number of iterations
	private int numberOfIterations;

	// error message 
	private String errorMessage = null;

	// number of threads to be created 
	private int numberOfThreads;
	
	// Was there some functionality error that deserves that this test get
	// a score of '0'?
	private boolean giveZeroScore = false;

	/**
	 * create threads of the given priority
	 * @param priority - Thread priority
	 * @param startIndex - start index of the array of threads
	 * @param endIndex - end index of the array of threads
	 */
	public void createStartThreads(PriorityType priority, int startIndex, int endIndex) {
		WorkerThread.setNumberOfIterations(numberOfIterations);
		for (int i = startIndex; i <= endIndex; i++) {
			t[i] = new WorkerThread(priority);
			t[i].setWorkQuantum(MATRIX_SIZE);
			t[i].start();
		}
	}

	/**
	 * returns true if the test results are consistent else false 
	 * @param t  Array of TestResults
	 * @return  true if test results are consistent else false
	 */
	public boolean checkResult(TestResults[] t) {
		boolean testPassStatus = true;

		double highPriorityMinTime = t[0].getMin();
		double mediumPriorityMinTime = t[1].getMin();
		double lowPriorityMinTime = t[2].getMin();

		if ( mediumPriorityMinTime > lowPriorityMinTime) {
			testPassStatus = false;
			errorMessage = "Atleast 1 medium priority thread took more time than low priority thread";
		} else if (highPriorityMinTime > mediumPriorityMinTime ) {     
			errorMessage = " Atleast 1 high priority thread took more time than medium/low priority thread";
			testPassStatus = false;
		}
		return testPassStatus;
	}

	/**
	 *  returns an array of test results 
	 *  @param tInfo   TestInfo object containg the test information
	 *  @param rtValues  RealTimeValues object containing the real-time conditional values set in the configuration file 
	 *  @param tParams  Test Parameters object
	 *  @return  array of test results
	 * @throws NotEligibleException 
	 **/
	@Override
	public TestResults[] benchMarkTest(TestInfo tInfo, RealTimeValues rtValues,
			TestParams tParams) throws NotEligibleException {
		/* Other variables local to this function */

		int new_iters;
		new_iters = updateIterations(tInfo, rtValues, tParams);

		numberOfIterations = new_iters;
		
		VerbosePrint.testStart(System.out, tParams, name(), tInfo.getTestIterations());
		
		int onlineCpus = Runtime.getRuntime().availableProcessors();
		if (onlineCpus == 1) {
			throw new NotEligibleException ("ThreadPriorityTest: Skipped, this " +
			"test is not run on a uniprocessor system");
		}

		SyncCounter counter = SyncCounter.getSyncCounter();
		Object lock = WorkerThread.getLock();

		numberOfThreads = 3 * onlineCpus;
		t = new WorkerThread[numberOfThreads];
		TestResults[] testResult = new TestResults[numberOfThreads];

		for (int i = 0; i < 3; i++) {
			// We want to store 1 result per thread
			testResult[i] = new TestResults(
					TestId.THREAD_PRIORITY_TEST,onlineCpus, true);
		}

		createStartThreads(PriorityType.HighPriority, 0, onlineCpus - 1);
		createStartThreads(PriorityType.MedPriority, onlineCpus, 2 * onlineCpus - 1);
		createStartThreads(PriorityType.LowPriority, 2 * onlineCpus, 3 * onlineCpus - 1);

		/* wait for every thread to start */
		while (counter.getCounter() != numberOfThreads) {
			try {
				synchronized (lock) {
					lock.wait(10000);
				}
			} catch (Exception e) {
				e.printStackTrace();
			}
		}

		WorkerThread.setStartStatus(true);

		synchronized (lock) {
			lock.notifyAll();
		}

		while (counter.getCounter() != 0) {
			synchronized( lock ) {
				try{
					lock.wait(10000);
				} catch (Exception e) {
					e.printStackTrace();
				}
			}
		}

		for (int i = 0; i < numberOfThreads; i++) {
			int resultBucket = i / onlineCpus;
			testResult[resultBucket].setValuesAtNextIndex(t[i].getStopTime());
			VerbosePrint.streamPrintln(System.out, tParams, 2,
					"Thread Priority: Stop Time of thread " + i + "= "
					+ t[i].getStopTime());
			try {
				t[i].join();
			} catch (Exception e) {
				e.printStackTrace();
			}
		}
		boolean isTestPass = checkResult(testResult);
		if (!isTestPass) {
			VerbosePrint.errPrintln ("ThreadPriorityTest: *** ERROR *** " + errorMessage);
			giveZeroScore = true;
		}
		
		TestResults[] returnResult = new TestResults[ 1 ];
		returnResult[0] = new TestResults( TestId.THREAD_PRIORITY_TEST, onlineCpus, true);
		returnResult[0].setTestResultName("Test result for high Priority Thread");

		for(int i = 0 ; i < onlineCpus; i++) {
			long difference = t[i].getStopTime( ) - t[i].getStartTime();
			difference /= new_iters;
			VerbosePrint.diffTime(System.out, tParams, name(), difference);					

			returnResult[0].setValuesAtNextIndex(difference);
		}
		return returnResult;
	}
	
	@Override
	public TestScores [] generateCombinedScore (TestInfo tInfo, TestResults [] tResult) {
		final double zero = 0;
		TestScores [] tScores = null;
		if (giveZeroScore) {
			tScores = new TestScores[1];
			tScores[0] = new TestScores();
			tScores[0].setTestResults(tResult);
			tScores[0].setThroughputScoreForTest (zero);
			tScores[0].setMaxThroughput(zero);
			tScores[0].setMinThroughput(zero);
			tScores[0].setMeanThroughput(zero);
			tScores[0].setDeterminismScore(zero);
			tScores[0].setWeightedThroughput(zero);
			return tScores;
		} else {
			return super.generateCombinedScore(tInfo, tResult);
		}
	}

	@Override
	public String name() {
		return "ThreadPriorityTest";
	}	

}

