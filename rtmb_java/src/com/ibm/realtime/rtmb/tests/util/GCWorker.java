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
import com.ibm.realtime.rtmb.tests.GarbageGenerationTest;
import com.ibm.realtime.rtmb.tests.TimerConfig;
import com.ibm.realtime.rtmb.util.VerbosePrint;

public class GCWorker implements Runnable {
	private int subIterations;
	private static final int MATRIX_SIZE = 5;
	private TestResults testResults;
	private TestInfo tInfo;
	private TestParams tParams;

	public GCWorker(TestResults t, TestInfo tInfo, TestParams tParams, int subIterations) {
		this.testResults = t;
		this.tInfo = tInfo;
		this.tParams = tParams;
		this.subIterations = subIterations;
	}

	public void run() {
		long startTime, endTime;
		long timeDiff = 0;
		int sleepTime = (int) TimerConfig.getWorkQuantum();
		for (int i = 0; i < tInfo.getTestIterations(); i++) {
			startTime = System.nanoTime();
			doTask();
			endTime = System.nanoTime();
			timeDiff = endTime - startTime;
			testResults.lockedSetValuesAtNextIndex(timeDiff);
			VerbosePrint.streamPrintln(System.out, tParams, 2, 
					"GarbageGenerationTest: " + Thread.currentThread().getName() 
					+ " - Difference between end and start times: ");
			VerbosePrint.streamPrintf(System.out, tParams, 2, "%.3f us" + 
					VerbosePrint.getLineSeparator(), VerbosePrint.roundToMicroSec(timeDiff));
			/*
			 * Sleep for a bit to help GC threads to get some time.
			 */
			try {
				long nanos = sleepTime*2;
				long millis = nanos / 1000000;
				if (millis > 0) {
					nanos %= 1000000;
				}
				Thread.sleep(millis, (int)nanos);
			} catch (InterruptedException e) {
			}
		}
		GarbageGenerationTest.decrementCounter();
	}

	private void doTask() {
		for (int i = 0; i < this.subIterations; i++) {
			MatrixOperationsWithObjects matrixOperations = new MatrixOperationsWithObjects(MATRIX_SIZE);
			matrixOperations.mathOnMatrices();
		}
	}
}
