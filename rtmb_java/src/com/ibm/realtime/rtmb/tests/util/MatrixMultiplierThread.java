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

import com.ibm.realtime.rtmb.tests.impl.BenchMarkThreadFactory;


/**
 * This thread class contains the logic to do quantum of work as determined by
 * the clock configuration tests. This class is used for thread priority tests.
 */
public class MatrixMultiplierThread implements Runnable {
	// lock object to be shared by all threads.
	private static Object lock = new Object();

	// boolean flag indicating whether the threads can start the task
	private static volatile boolean canStartWork = false;

	// start time of the task
	private long[] startTime = null;

	// stop time of the task
	private long[] endTime = null;

	// thread wrapper
	private BenchMarkThread benchMarkThread;

	// number of iterations
	int iterations;

	// number of subIterations
	int subIterations;

	// size of matrix
	int size = 10;

	/**
	 * returns the start time of task
	 * 
	 * @return start time of task
	 */
	public long[] getStartTime() {
		return startTime;
	}

	/**
	 * returns the stop time of task
	 * 
	 * @return stop time of task
	 */
	public long[] getEndTime() {
		return endTime;
	}

	/**
	 * returns the shared lock
	 * 
	 * @return the static lock object
	 */
	public static Object getLock() {
		return lock;
	}

	/**
	 * Constructor
	 * 
	 * @param priority
	 *            - priority to be set for the thread
	 * @param iterations
	 *            - number of iterations to run
	 * @param subIterations
	 *            - number of subIterations to run
	 */
	public MatrixMultiplierThread(PriorityType priority, int iterations,
			int subIterations) {
		benchMarkThread = BenchMarkThreadFactory.getInstance().getThread(
				priority, this);
		startTime = new long[iterations];
		endTime = new long[iterations];
		this.iterations = iterations;
		this.subIterations = subIterations;
	}

	/**
	 * Set the status flag
	 * 
	 * @param flag
	 *            boolean value indicating whether the thread can continue with
	 *            its task or not
	 */
	public static void setStartStatus(boolean flag) {
		canStartWork = flag;
	}

	/**
	 * execute the thread task
	 */
	public void run() {
		SyncCounter counter = SyncCounter.getSyncCounter();
		int matrixA[][] = new int[size][size];
		int matrixB[][] = new int[size][size];
		int matrixC[][] = new int[size][size];

		Matrix.randomFill(matrixA, size, 1000);
		Matrix.randomFill(matrixB, size, 1000);

		synchronized (lock) {
			counter.incrementCounter();

			while (canStartWork == false) {
				try {
					lock.wait();
				} catch (Exception e) {
					e.printStackTrace();
				}
			}
		}

		for (int i = 0; i < iterations; i++) {
			startTime[i] = System.nanoTime();
			for (int j = 0; j < subIterations; j++) {
				Matrix.multiply(matrixA, matrixB, matrixC, size);
			}
			endTime[i] = System.nanoTime();
		}

		synchronized (lock) {
			counter.decrementCounter();

			if (counter.getCounter() == 0)
				lock.notify();
		}

	}

	public void start() {
		benchMarkThread.execute();
	}

	public void join() throws InterruptedException {
		benchMarkThread.waitForThreadExit();
	}
}
