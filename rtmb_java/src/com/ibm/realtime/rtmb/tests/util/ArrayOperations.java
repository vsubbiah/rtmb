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

public class ArrayOperations {
	private static final int PAGE_SIZE = 4 * 1024;

	/*
	 * Do not allow creation of instances.
	 */
	private ArrayOperations() {
	}

	/**
	 * do_array_opern() - create maximum cache misses and 
	 * force memory reads.
	 * @param m - byte array for random memory access.
	 * @param size - size of the array.
	 */

	public static int do_array_opern (byte[] m, int subIters) {
		long i;
		int size = m.length;
		int index = 0;
		assert ((size != 0) && (size > (PAGE_SIZE + 1)));
		/* Force access of another page */
		for (i = 0; i < subIters; i++) {
			index = (int) ( ((i + 1) * (100 * PAGE_SIZE) ) % m.length);
			m[index] = 'x';
		}
		return index;
	}
}