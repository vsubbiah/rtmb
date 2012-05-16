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
import com.ibm.realtime.rtmb.tests.util.Matrix;
import com.ibm.realtime.rtmb.tests.util.MatrixMultiplierThread;
import com.ibm.realtime.rtmb.tests.util.NotEligibleException;
import com.ibm.realtime.rtmb.tests.util.PriorityType;
import com.ibm.realtime.rtmb.tests.util.SpecTest;
import com.ibm.realtime.rtmb.tests.util.SyncCounter;
import com.ibm.realtime.rtmb.tests.util.TestId;
import com.ibm.realtime.rtmb.util.VerbosePrint;
/** 
 * This thread class contains the code to run thread priority tests.
 * This class will create High ,medium and low priority threads simultaneously .
 * Based on the time taken for completion of tasks determines whether the threads have completed  
 * according to priority.
 */
public class MultiThreadTest extends SpecTest {
	// array of worker threads 
	MatrixMultiplierThread t[];

	// number of threads to be created 
	int numberOfThreads;

	// number of iterations to run
	int iterations = 1;

	// number of subIterations to run 
	int subIterations = 2;

	// clock accuracy 
	double clockAccuracy=0;

	// online CPUs
	int onlineCPUs;

	/**
	 * create threads of the given priority
	 * @param priority - Thread priority
	 * @param startIndex - start index of the array of threads
	 * @param endIndex - end index of the array of threads
	 * @param iterations - number of iterations to run 
	 * @param subIterations - number of iterations to run
	 */
	public void createStartThreads(PriorityType priority, int startIndex, int endIndex ) {
		for (int i = startIndex; i <= endIndex; i++) {
			t[i] = new MatrixMultiplierThread(priority , iterations , subIterations );
			t[i].start();
		}
	}

	/**
	 * returns true if the test results are consistent else false 
	 * @param t  Array of TestResults
	 * @return  true if test results are consistent else false
	 */
	public boolean checkResult(TestResults[] threadResults , RealTimeValues rtValues, TestParams tParams,int iterations) {
		boolean rc = true;
		int i = 0;
		TestResults consolidator = threadResults[threadResults.length  - 1];
		consolidator.setTestResultName("Consolidated test result for all threads");
		consolidator.resetResults();

		for(i = 0 ; i <= threadResults.length - 2 ; i++) {
			VerbosePrint.streamPrintln(System.out, tParams, 1, "");
			VerbosePrint.streamPrintln(System.out, tParams, 1,
					"MultiThreadTest: Test Report for thread_" + i + ":");
			VerbosePrint.streamPrintln(System.out, tParams, 1,
			"==========================================");
			if (!threadResults[i].checkStdDev(rtValues, tParams)) {
				rc = false;
				break;
			} else {
				consolidator.setValuesAtNextIndex((long)threadResults[i].getMean());
			}
		}
		if( rc ) {
			VerbosePrint.streamPrintln(System.out, tParams, 1, "");
			VerbosePrint.streamPrintln(System.out, tParams, 1,
					"MultiThreadTest: " + consolidator.getTestResultName() + ":");
			VerbosePrint.streamPrintln(System.out, tParams, 1,
			"==========================================================");
			if (!consolidator.checkStdDev(rtValues, tParams)) {
				rc = false;
			}
		}
		return rc;
	}

	double getScore( TestResults[] tResult, int length) {
		double sum = 0;
		double reciprocal;

		for(int i = 0; i < length - 1 ; i++ ) {
			reciprocal = 1.0 / tResult[i].getMean();
			sum += reciprocal;
		}

		double perThreadThroughput = ( (1.0 / tResult[length-1].getMean()) * onlineCPUs) * 1000000000;
		double multicpuThroughput  = ( sum ) * 1000000000;

		double score = ( multicpuThroughput / perThreadThroughput ) * 50.0  ;

		if( score > 50.0) { /* max score for throughput is 50 */
			score = 50.0;
		}

		return score;
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
	public TestResults[] benchMarkTest(TestInfo tInfo, RealTimeValues rtValues,	TestParams tParams) 
	throws NotEligibleException {
		/* Other variables local to this function */
		int count = 0;
		clockAccuracy = ClockConfig.getClockAccuracy();

		iterations = updateIterations(tInfo, rtValues, tParams);

		VerbosePrint.testStart(System.out, tParams, name(), tInfo.getTestIterations());
		
		onlineCPUs = Runtime.getRuntime().availableProcessors();

		if (onlineCPUs == 1) {
			throw new NotEligibleException ("MultiThreadTest: Skipped, reason: Number of CPU's available is only 1");
		}

		SyncCounter counter = SyncCounter.getSyncCounter();
		Object lock = MatrixMultiplierThread.getLock();

		numberOfThreads = onlineCPUs + 1;
		t = new MatrixMultiplierThread[numberOfThreads];
		TestResults[] testResult = new TestResults[numberOfThreads + 1];

		for (int i = 0; i < numberOfThreads ; i++) {
			// We want to store 1 result per thread
			testResult[i] = new TestResults(
					TestId.MULTI_THREAD_TEST, tInfo.getTestIterations(), true);
			testResult[i].setTestResultName("thread_" + 1);
		}

		/* consolidator result will have only  " numberOfThreads" values */
		testResult[numberOfThreads] = new TestResults(
				TestId.MULTI_THREAD_TEST,numberOfThreads, true);  
		/* we are not creating additional array. use  existing values array 
		 * and clean the array before proceeding with the test 
		 */
		sampleRun(testResult[ numberOfThreads - 1],rtValues,tParams);
		testResult[numberOfThreads - 1 ].resetResults();

		VerbosePrint.streamPrintln(System.out, tParams, 2,
				" MultiThreadTest: Number of iterations= " + tInfo.getTestIterations() +  "Subiterations = " 
				+ subIterations );

		while (true) {
			if (count != 0) {
				VerbosePrint.testRetry(System.out, tParams, name());
			}
			if (count++ >= tInfo.getThreshold()) {
				break;
			}
			counter.setCounter(0);
			for (int i = 0; i < numberOfThreads ; i++) {
				testResult[i].resetResults();
			}

			MatrixMultiplierThread.setStartStatus(false);
			createStartThreads( PriorityType.HighPriority, 0, numberOfThreads - 2);

			/* wait for every thread to start */
			while (counter.getCounter() != ( numberOfThreads - 1))  {
				try {
					Thread.sleep(2000);
				} catch (Exception e) {
					e.printStackTrace();
				}
			}

			/* set the start status flag  to true so that Thread continues after receiving a notification */
			MatrixMultiplierThread.setStartStatus(true);

			synchronized (lock) {
				lock.notifyAll();
				try {
					lock.wait();
				} catch(Exception e) {
					e.printStackTrace();
				}
			}

			MatrixMultiplierThread.setStartStatus(false);
			createStartThreads( PriorityType.HighPriority, numberOfThreads - 1, numberOfThreads - 1 );

			/* set the start status flag  to true so that Thread continues after receiving a notification */

			/* wait for every thread to start */
			while (counter.getCounter() != 1)  {
				try {
					Thread.sleep(1000);
				} catch (Exception e) {
					e.printStackTrace();
				}
			}

			synchronized(lock) {
				MatrixMultiplierThread.setStartStatus(true);
				lock.notifyAll();
				try {
					lock.wait();
				} catch(Exception e)  {
					e.printStackTrace();
				}
			}

			for (int i = 0; i < numberOfThreads; i++) {
				try {
					t[i].join();
				} catch (Exception e) {
					e.printStackTrace();
				}

				/* collect the results from each threads */
				long[] startTime = t[i].getStartTime();
				long[] endTime = t[i].getEndTime();

				String threadName = " thread#" + i; 
				for( int  j = 0 ; j < iterations ; j++) {
					long difference  =  endTime[j] - startTime[j] ;
					testResult[i].setValuesAtNextIndex(difference);
					VerbosePrint.streamPrint (System.out, tParams, 2,"MultiThreadTest: " + 
							threadName + " Difference between end and start times = ");
					VerbosePrint.streamPrintf(System.out, tParams, 2, "%.3f us" + 
							VerbosePrint.getLineSeparator(), 
							VerbosePrint.roundToMicroSec(difference));
				}
			}
			if (checkResult(testResult ,rtValues, tParams,iterations)) {
				break;
			}
			subIterations *= 2;
		}
		return testResult;
	}

	public void sampleRun( TestResults result ,RealTimeValues rtValues, TestParams tParams) {
		long startTime , endTime, diffTime;

		int retryNumber = -1;
		boolean done = false;
		int size=10;

		int data1[][] = new int [size][size];
		int data2[][] = new int [size][size];
		int data3[][] = new int [size][size];

		Matrix.randomFill(data1, size, 1000);
		Matrix.randomFill(data2, size, 1000);

		tParams.setVerbose(false);
		while(!done) {
			retryNumber ++;
			result.resetResults();

			for (int i = 0; i < iterations ; i++) {
				startTime = System.nanoTime();

				for(int j = 0 ; j < subIterations ; j++ ) {
					Matrix.multiply(data1,data2,data3,size);
				}
				endTime = System.nanoTime();
				diffTime = endTime - startTime;
				result.setValuesAtNextIndex(diffTime);
			}
			if( result.getMedian() > clockAccuracy ) {
				done = true;
			} else {
				subIterations *= 2;
			}
		}
		tParams.setVerbose(true);
	}

	@Override
	public TestScores [] generateCombinedScore (TestInfo tInfo, TestResults [] results) {
		TestResults[] tResult = new TestResults[results.length - 2];
		int i = 0;

		for(i= 0 ; i < results.length - 2; i ++ )
			tResult[i] = results[i];

		TestScores[] tScore = super.generateCombinedScore(tInfo, tResult);

		double product = 1; 
		double sum = 0;

		for(i = 0 ; i < tResult.length ; i++) {
			product *= Math.pow(tResult[i].getMedian() , 1.0 / tResult.length);
			sum += tResult[i].getMedian();
		}
		double parallelism = sum / (onlineCPUs * results[i].getMedian());
		double score  = tInfo.getReferenceScore() / (product / parallelism);
		tScore[0].setThroughputScoreForTest(score);
		return tScore;
	}
	
	@Override
	public String name() {
		return "MultiThreadTest";
	}
		
}

