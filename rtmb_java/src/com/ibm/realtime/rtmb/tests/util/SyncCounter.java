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

/**
 * This class is used to increment or decrement counter value
 */
public class SyncCounter {

	private static SyncCounter sync = new SyncCounter();
	int counter = 0;

	/**
	 * Constructor
	 */
	private SyncCounter() {
	}

	/**
	 * @return singleton object for SyncCounter class
	 */
	public static SyncCounter getSyncCounter() {
		return sync;
	}

	/**
	 * @return current value stored in the counter
	 */
	public int getCounter() {
		return counter;
	}

	/**
	 * increments the value stored in the counter
	 */
	public void incrementCounter() {
		counter++;
	}

	/**
	 * decrements the value stored in the counter
	 */
	public void decrementCounter() {
		counter--;
	}

	/**
	 * sets the value stored in the counter
	 * 
	 * @param counterValue
	 *            sets counter to the specified value
	 */
	public void setCounter(int counterValue) {
		counter = counterValue;
	}
}
