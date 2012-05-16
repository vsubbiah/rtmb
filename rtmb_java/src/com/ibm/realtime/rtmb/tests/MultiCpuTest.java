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

import java.io.BufferedReader;
import java.io.File;
import java.io.FileNotFoundException;
import java.io.FileReader;
import java.io.IOException;
import java.text.SimpleDateFormat;
import java.util.Date;

import com.ibm.realtime.rtmb.params.RealTimeValues;
import com.ibm.realtime.rtmb.params.TestInfo;
import com.ibm.realtime.rtmb.params.TestParams;
import com.ibm.realtime.rtmb.results.TestResults;
import com.ibm.realtime.rtmb.results.TestScores;
import com.ibm.realtime.rtmb.tests.impl.BenchMarkThreadFactory;
import com.ibm.realtime.rtmb.tests.util.BenchMarkThread;
import com.ibm.realtime.rtmb.tests.util.Cpus;
import com.ibm.realtime.rtmb.tests.util.MatrixOperationsWithObjects;
import com.ibm.realtime.rtmb.tests.util.MultiCpuHelperThread;
import com.ibm.realtime.rtmb.tests.util.NotEligibleException;
import com.ibm.realtime.rtmb.tests.util.PriorityType;
import com.ibm.realtime.rtmb.tests.util.SpecTest;
import com.ibm.realtime.rtmb.tests.util.TestId;
import com.ibm.realtime.rtmb.tests.util.TestIncompleteException;
import com.ibm.realtime.rtmb.util.VerbosePrint;

public class MultiCpuTest extends SpecTest implements Runnable  {
	// number of iterations
	int iterations = 1;

	// number of subIterations
	int subIterations = 1;

	int counter = 0;
	TestResults[] testResult = null;

	private final static int matrixSize = 10;

	/**
	 * returns an array of test results
	 * 
	 * @param tInfo
	 *            TestInfo object containing the test information
	 * @param rtValues
	 *            RealTimeValues object containing the real-time conditional
	 *            values set in the configuration file
	 * @param tParams
	 *            Test Parameters object
	 * @return array of test results
	 **/

	public synchronized void incrementCounter() {
		counter++;
	}

	public synchronized void decrementCounter() {
		counter--;
	}

	public synchronized int getCounter() {
		return counter;
	}

	public String getOutputDir( TestParams tParams )  {
		String rptDir = System.getProperty("java.io.tmpdir");

		if( rptDir == null ||  rptDir ==   "" ) {
			rptDir = "."  ;
		}
		return rptDir;
	}

	@Override
	public TestResults[] benchMarkTest(TestInfo tInfo, RealTimeValues rtValues,
			TestParams tParams) throws NotEligibleException, TestIncompleteException {
		/* Other variables local to this function */

		int new_iters;
		int count = 0;
		boolean retryFlag = false;

		new_iters = updateIterations(tInfo, rtValues, tParams);

		iterations = new_iters;
		int onlineCpus = Cpus.onlineCpus;
		if (onlineCpus == 1) {
			throw new NotEligibleException ("MultiCpuTest: Skipped, this test is " +
			"not run on a uniprocessor system");
		}

		VerbosePrint.testStart(System.out, tParams, name(), tInfo.getTestIterations());
		
		testResult = new TestResults[onlineCpus + 1];

		for (int i = 0; i < onlineCpus ; i++) {
			// We want to store 1 result per thread
			testResult[i] = new TestResults(TestId.MULTI_CPU_TEST, iterations, true);
			testResult[i].setTestResultName("Process " + i);
		}
		testResult[onlineCpus] = new TestResults(TestId.MULTI_CPU_TEST, onlineCpus, true);
		TestResults summaryResult = testResult[onlineCpus];

		VerbosePrint.streamPrintln(System.out, tParams, 2,
				"MultiCPU: Number of iterations: " + tInfo.getTestIterations());
		BenchMarkThread thrd = BenchMarkThreadFactory.getInstance().getThread(
				PriorityType.HighPriority, this);
		tParams.setVerbose(false);
		try {
			thrd.execute();
			thrd.waitForThreadExit();
		} catch (Exception e) {
			e.printStackTrace();
			/* sample run failed */
			VerbosePrint.errPrintln();
			VerbosePrint.errPrintln("MultiCPU: Sample run failed");
			VerbosePrint.errPrintln();
			tParams.setVerbose(true);
			return testResult;
		}
		
		tParams.setVerbose(true);
		double values[] = testResult[0].getValues();
		for (int i = 0; i < iterations; i++) {
			VerbosePrint.streamPrint(System.out, tParams, 2,
					"MultiCPU: Sample run #" + i + " Time difference between end and start time = ");
			VerbosePrint.streamPrintf(System.out, tParams, 2, "%.3f us" + 
					VerbosePrint.getLineSeparator(), VerbosePrint.roundToMicroSec(values[i]));
		}
		long sleepTime = (long)(testResult[0].getMean() + 0.5);
		
		testResult[0].resetResults();
		while (true) {
			if (count++ >= tInfo.getThreshold()) {
				break;
			}

			MultiCpuHelperThread processRunner[] = new MultiCpuHelperThread[2 * onlineCpus];
			long now = System.nanoTime() / 1000000000 ;

			/* give 5 second of start time per process. crude way .. should refine it later */
			long timeToStart = now + (5 * onlineCpus) ;

			/* Start the low priority process */
			String dir = getOutputDir(tParams);
			SimpleDateFormat df = new SimpleDateFormat("yyyyMMdd'_'hhmmssSS");
			String date = df.format(new Date());
			String multiCpuTmpDirName = dir + File.separator + "multiCpu_" + date;
			File multiCpuTmpDir = new File (multiCpuTmpDirName);
			if (!multiCpuTmpDir.mkdirs()) {
				System.err.println("Internal Error. Unable to create directory: " + multiCpuTmpDir.getAbsolutePath());
				break;					
			}
			for (int i = 0; i < onlineCpus; i++) {
				processRunner[i] = new MultiCpuHelperThread(subIterations,
						iterations, sleepTime , timeToStart,
						multiCpuTmpDirName + File.separator + "multiCpu_" + i,
						PriorityType.LowPriority, this, tParams.getJvmOptions(), matrixSize);
				processRunner[i].start();
			}

			/* Start the high priority process */
			for (int i = onlineCpus; i < 2 * onlineCpus; i++) {
				processRunner[i] = new MultiCpuHelperThread(subIterations,
						iterations, sleepTime, timeToStart,
						multiCpuTmpDirName + File.separator + "multiCpu_" + i,
						PriorityType.HighPriority, this, tParams.getJvmOptions(), matrixSize);
				processRunner[i].start();
			}

			synchronized (this) {
				while ( getCounter() != 0 ) {
					try {
						wait();
					} catch (Exception e) {
						e.printStackTrace();
					}
				}
			}
			String fileName = null;
			/* Start new Threads to take care of executing the processes */
			for (int i = onlineCpus; i < 2 * onlineCpus; i++) {
				TestResults result = testResult[i - onlineCpus];
				fileName = processRunner[i].getFileName();
				if (scanFile(i,fileName, result, iterations, tParams) == false) {
					/* Hard failure */
					throw new TestIncompleteException ("MultiCPU: Test " +
							"failed due to error while reading file " + fileName, testResult);
				}
				VerbosePrint.outPrintln(System.out, tParams, 1);
				VerbosePrint.streamPrintln(System.out, tParams, 1,
						"Test Result for " + result.getTestResultName() + ":");
				VerbosePrint.streamPrintln(System.out, tParams, 1,
				"==========================");
				if (!result.checkStdDev(rtValues, tParams)) {
					retryFlag = true;
				} else {
					summaryResult.setValuesAtNextIndex((long)(result.getMean()+0.5));
				}
			}
			tParams.setVerbose(false);
			if (retryFlag || !summaryResult.checkStdDev(rtValues, tParams)) {
				tParams.setVerbose(true);
				retryFlag = false;
				subIterations *= 2;
				/*
				 * Reset all the TestResult objects.
				 */
				for (int i = onlineCpus; i < 2 * onlineCpus; i++) {
					testResult[i].resetResults();
				}
				/*
				 * Delete all the previously created files in /tmp dir.
				 */
				summaryResult.resetResults();
				for (int i = 0; i < 2 * onlineCpus; i++) {
					File fName = new File (multiCpuTmpDirName + File.separator + "multiCpu_" + i);
					if (!fName.delete()) {
						System.err.println("Internal Error. Unable to delete: " + fName.getAbsolutePath());
						break;
					}
				}
				if (!multiCpuTmpDir.delete()) {
					System.err.println("Internal Error. Unable to delete: " + multiCpuTmpDir.getAbsolutePath());
					break;
				}
				VerbosePrint.testRetry(System.out, tParams, name());
			} else {
				break;
			}
		}
		tParams.setVerbose(true);
		return testResult;
	}

	public boolean scanFile(int processId,String fileName, TestResults result, int iters,TestParams tParams) {
		boolean retVal = true;
		long time = 0;

		String process = "process" + processId;
		try {
			BufferedReader reader = new BufferedReader(new FileReader(fileName));
			for (int i = 0; i < iters; i++) {
				time = Long.parseLong(reader.readLine());
				result.setValuesAtNextIndex(time);
				VerbosePrint.streamPrint(System.out, tParams, 2, "MultiCpuTest: " + process + 
						" Time difference between end and start time = ");
				VerbosePrint.streamPrintf(System.out, tParams, 2, "%.3f us" + 
						VerbosePrint.getLineSeparator(), 
						VerbosePrint.roundToMicroSec(time));
			}
			reader.close();
		} catch (FileNotFoundException e) {
			e.printStackTrace();
			retVal = false;
		} catch (NumberFormatException e) {
			retVal = false;
			e.printStackTrace();
		} catch (IOException e) {
			retVal = false;
			e.printStackTrace();
		}
		/* retVal is true if we are able to scan and load the file contents */
		return retVal;
	}

	public void run() {
		TestResults result = testResult[0];
		int retryNumber = -1;
		long startTime, endTime;
		boolean done = false;

		MatrixOperationsWithObjects matrixOps = new MatrixOperationsWithObjects(
				matrixSize);
		while (!done) {
			retryNumber++;
			result.resetResults();
			for (int i = 0; i < iterations; i++) {
				startTime = System.nanoTime();
				for (int j = 0; j < subIterations; j++) {
					matrixOps.matrixMultiplication();
				}
				endTime = System.nanoTime();
				result.setValuesAtNextIndex(endTime - startTime);
			}

			/*
			 *  Median should be more than 1 units of clock Accuracy and 2 units of time Accuracy
			 */
			if (result.getMedian() >  ClockConfig.getClockAccuracy() && ( 2 *  result.getMedian() )  >  ( TimerConfig.getTimerAccuracy())) {
				done = true;
			} else {
				done = false;
				subIterations *= 2;
			}
		}
	}

	@Override
	public TestScores [] generateCombinedScore (TestInfo tInfo, TestResults [] results) {
		TestResults[] tResult = new TestResults[results.length - 1];
		int i =0;

		/* 
		 * last element in the result array is actually the consolidator .. dont use it 
		 * for score generation
		 */
		for(i= 0 ; i < results.length - 1; i ++ ) {
			tResult[i] = results[i];
		}
		TestScores[] tScore = super.generateCombinedScore(tInfo, tResult);
		return tScore;
	}
	
	@Override
	public String name() {
		return "MultiCpuTest";
	}
		
}

