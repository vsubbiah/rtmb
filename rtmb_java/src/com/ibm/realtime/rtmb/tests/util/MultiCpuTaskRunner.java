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

import java.io.FileNotFoundException;
import java.io.PrintStream;

import com.ibm.realtime.rtmb.tests.impl.BenchMarkThreadFactory;

public class MultiCpuTaskRunner implements Runnable {
	private int iterations;
	private int subIterations;
	private long sleepTime;
	private long timeToStart;
	private String name;
	private boolean isLogNeeded;
	private BenchMarkThread thrd;
	int sleepUnits;
	int workUnits;

	MatrixOperationsWithObjects matrixOps = null;

	public MultiCpuTaskRunner(int iterations, int subIterations,
			long timeToStart, long sleepTime, String name, int priority,
			int matrixSize) {
		this.subIterations = subIterations;
		this.iterations = iterations;
		this.timeToStart = timeToStart;
		this.sleepTime = sleepTime;
		this.name = name;

		if (priority == 1) {
			isLogNeeded = true;
			sleepUnits = 4;
			workUnits = 1;
		} else {
			isLogNeeded = false;
			sleepUnits = 2;
			workUnits = 2;
		}
		matrixOps = new MatrixOperationsWithObjects(matrixSize);
	}

	public void doWork() {
		thrd = BenchMarkThreadFactory.getInstance().getThread(
				PriorityType.HighPriority, this);
		thrd.execute();
		try {
			thrd.waitForThreadExit();
		} catch (Exception e) {
			e.printStackTrace();
		}
	}

	public void run() {
		long startTime = 0, endTime = 0, diffTime = 0;
		long timeDiff[] = new long[iterations];
		PrintStream out = null;

		if (isLogNeeded) {
			try {
				out = new PrintStream(name);
			} catch(FileNotFoundException f) {
				f.printStackTrace();
				return;
			}
		}

		long millisecs = 0;
		int nanosecs = 0;
		long megas = 1000000;
        long nanos = 1000000000;
		long now = System.nanoTime( ) / nanos ;
		long nanosToSleep = ( timeToStart  - now ) * nanos;
		millisecs = nanosToSleep / megas;
		nanosecs = (int) (nanosToSleep % megas);

		try {
			Thread.sleep(millisecs, nanosecs);
		} catch (Exception e) {
			e.printStackTrace();
			out.close();
			return;
		}

		nanosToSleep = sleepUnits * sleepTime;
		millisecs = nanosToSleep / megas;
		nanosecs = (int) (nanosToSleep % megas);

        subIterations *= workUnits;

        for (int i = 0; i < iterations; i++) {
                int j = subIterations;    
                startTime = System.nanoTime();

                while( j > 0) {
                        matrixOps.matrixMultiplication();
                        try {
                                Thread.sleep(millisecs, nanosecs);
                        } catch (Exception e) {
                                e.printStackTrace();
                        }
                        j--;
                }
                endTime = System.nanoTime();
                diffTime = endTime - startTime;
                timeDiff[i] = diffTime;
        }

		if (isLogNeeded) {
			for (int i = 0; i < iterations; i++) {
				out.println(timeDiff[i]);
			}
		}
		if (out != null) {
			out.close();
		}
	}
}
