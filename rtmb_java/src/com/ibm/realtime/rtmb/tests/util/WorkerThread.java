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
public class WorkerThread implements Runnable {

	// lock object to be shared by all threads.
	private static Object lock = new Object();

	// quantum of work to be done
	private int workQuantum = 0;

	// boolean flag indicating whether the threads can start the task
	private static boolean canStartWork = false;

	// number of times to call addNumbers routine
	private static int numberOfIterations = 0;

	// start time of the task
	private long startTime;

	// stop time of the task
	private long stopTime;

	// thread wrapper
	private BenchMarkThread benchMarkThread;

	// thread factory
	private BenchMarkThreadFactory threadFactory = BenchMarkThreadFactory
	.getInstance();

	/**
	 * Set start time of the task done by the thread
	 * 
	 * @param start
	 *            the start time of the task
	 */
	public void setStartTime(long start) {
		this.startTime = start;
	}

	/**
	 * Set stop time of the task done by the thread
	 * 
	 * @param stop
	 *            the end time of the task
	 */
	public void setStopTime(long stop) {
		this.stopTime = stop;
	}

	/**
	 * returns the start time of task
	 * 
	 * @return start time of task
	 */
	public long getStartTime() {
		return startTime;
	}

	/**
	 * returns the stop time of task
	 * 
	 * @return stop time of task
	 */
	public long getStopTime() {
		return stopTime;
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
	 * Set the number of iterations to run
	 * 
	 * @param iterationCount
	 *            - the number of iterations to run
	 */

	public static synchronized void setNumberOfIterations(int iterationCount) {
		numberOfIterations = iterationCount;
	}

	/**
	 * Constructor
	 * 
	 * @param PriorityParams
	 *            - priority to be set for the thread
	 */
	public WorkerThread(PriorityType priority) {
		benchMarkThread = threadFactory.getThread(priority, this);

	}

	/**
	 * Set the status flag
	 * 
	 * @param flag
	 *            boolean value indicating whether the thread can continue with
	 *            its task or not
	 */
	public static void setStartStatus(boolean flag) {
		canStartWork = true;
	}

	/**
	 * Set the work quantum
	 * 
	 * @param workQuantum
	 *            - Quantum of work to be executed by the thread
	 */
	public void setWorkQuantum(int workQuantum) {
		this.workQuantum = workQuantum;
	}

	/**
	 * execute the thread task
	 */
	public void run() {
		MatrixOperationsWithPrimitives mowp = new MatrixOperationsWithPrimitives(workQuantum);
		startTime = System.nanoTime();
		SyncCounter counter = SyncCounter.getSyncCounter();

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

		for (int i = 0; i < numberOfIterations; i++) {
			mowp.matrixMultiplication();
		}

		synchronized (lock) {
			counter.decrementCounter();

			if (counter.getCounter() == 0) {
				lock.notify(); /* wake up the parent */
			}
		}
		stopTime = System.nanoTime();
	}

	public void start() {
		benchMarkThread.execute();
	}

	public void join() throws InterruptedException {
		benchMarkThread.waitForThreadExit();
	}
}
