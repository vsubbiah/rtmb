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
 
#include <ArrayOperations.h>
#include <assert.h>

/*
 * Name    : doArrayOpern()
 * Desc    : This function is meant to create maximum
 *           cache misses and force memory reads.
 *           This test will help get a measure of memory
 *           access performance.
 * Args    : m    = array of chars.
 *           size = size of the array m.
 * Returns : 0 - on success; -1 on failure.
 */

void ArrayOperations::doArrayOpern(int size) {
	int j = 0, i = 0;
	assert ((size != 0) && (size > RTMB_PAGESIZE + 1));
	char *m = new char[size];

	/* Force access of another page */
	int j_inc = RTMB_PAGESIZE + 1;
	int end = (size / j_inc);

	for (i = 0; i < end; i++, j += j_inc) {
		if (i % 2 == 0) {
			m[j] = 'a';
		} else {
			m[end - i - 1] = 'b';
		}
	}
	delete[] m;
}
