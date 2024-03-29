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
 
package com.ibm.realtime.rtmb.tests.util;

import com.ibm.realtime.rtmb.params.TestInfo;
import com.ibm.realtime.rtmb.params.TestParams;
import com.ibm.realtime.rtmb.results.TestResults;
import com.ibm.realtime.rtmb.tests.ClockConfig;
import com.ibm.realtime.rtmb.tests.CompilationTest;
import com.ibm.realtime.rtmb.tests.TimerConfig;
import com.ibm.realtime.rtmb.util.NoPreCompileAnnotation;
import com.ibm.realtime.rtmb.util.VerbosePrint;


@NoPreCompileAnnotation
public class CompilationWorker implements Runnable {
	private TestResults testResults;
	private int iterations;
	private boolean isUniProcessor;
	private TestParams tParams;
	private TestInfo tInfo;
	private static final int MATRIX_SIZE = 50;

	public CompilationWorker (TestResults tResult, TestInfo tInfo, int iterations, boolean isUniProcessor, TestParams tParams) {
		this.testResults = tResult;
		this.iterations = iterations;
		this.isUniProcessor = isUniProcessor;
		this.tParams = tParams;
		this.tInfo = tInfo;
	}
	public void run() {
		long startTime, endTime;
		long timeDiff = 0;
		int sleepTime = 0;
		int subIters = 1;
		int count = 0;
		double clockMonotonicity = ClockConfig.getClockAccuracy();

		MatrixOperationsWithPrimitives matrixOperations = new MatrixOperationsWithPrimitives(MATRIX_SIZE);
		/*
		 * If we are running on a uniprocessor machine, then sleep for
		 * the minimum duration that the system is configured with.
		 * We will consistently using the configured sleep time instead
		 * of randomly hard coding the amount of sleep that a thread needs
		 * to do when required. 
		 */
		if (isUniProcessor) {
			sleepTime = (int) TimerConfig.getWorkQuantum();
		}

		/*
		 * Iterate over this loop doing a matrix multiplication
		 * each time.
		 */
		while (true) {
			if (count++ < tInfo.getThreshold()) {
				for (int i = 0; i < iterations; i++) {
					//BenchMarkClock startTime = clockFactory.getClock();
					startTime = System.nanoTime();
					for (int j = 0; j < subIters; j++) {
						doTask (matrixOperations);					
					}
					//BenchMarkClock endTime = clockFactory.getClock();
					endTime = System.nanoTime();
					//timeDiff = endTime.diffTime(startTime.getTime());
					timeDiff = endTime - startTime;
					VerbosePrint.streamPrint (System.out, tParams, 2, "CompilationTest: " + 
							Thread.currentThread().getName() + ":" + " Difference between end " +
					"and start times = ");
					VerbosePrint.streamPrintf(System.out, tParams, 2, "%.3f us" + VerbosePrint.getLineSeparator(), 
							VerbosePrint.roundToMicroSec(timeDiff));
					testResults.lockedSetValuesAtNextIndex(timeDiff);
					if (isUniProcessor) {
						try {
							Thread.sleep(0,sleepTime);
						} catch (InterruptedException e) {
							// Do nothing.
						}
					}
				}
				if (testResults.getMin() > clockMonotonicity) {
					break;
				} else {
					subIters*=2;
					testResults.resetResults();
					VerbosePrint.streamPrintln (System.out, tParams, 1,
					"\nCompilation: Retrying test with a bigger work quantum...");
				}
			} else {
				break;
			}
		}
		CompilationTest.decrementCounter();
	}

	private int[][] doTask(MatrixOperationsWithPrimitives matrixOperns) {
		return matrixOperns.matrixMultiplication();
	}
}
