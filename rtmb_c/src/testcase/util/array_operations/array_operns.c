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
 
#include <array_operns.h>

/*
 * Name    : do_array_opern()
 * Desc    : This function is meant to create maximum
 *	   cache misses and force memory reads.
 *	   This test will help get a measure of memory
 *	   access performance.
 * Args    :    m    = array of chars.
 * 		size = size of the array m.
 * Returns : 0 - on success; -1 on failure.
 */

int do_array_opern(char *m, int size) {
	int j = 0, i = 0;

	/* Force access of another page */
	int j_inc = RTMB_PAGE_SIZE + 1;
	int end = (size / j_inc);

	/* Make sure array is alloced */
	if (m == NULL) {
		RTMB_printf(stderr, "do_array_opern: (null) array passed\n");
		return ERROR;
	}
	for (i = 0; i < end; i++, j += j_inc) {
		if (i % 2 == 0) {
			m[j] = 'a';
		} else {
			m[end - i - 1] = 'b';
		}
	}
	return SUCCESS;
}

/****************************** Unused ************************************
 void do_matrix_mul() {
 int i, j;
 int mat1[MATRIX_SIZE][MATRIX_SIZE] = { {1,2,3}, {4,5,6}, {7,8,9} };
 int mat2[MATRIX_SIZE][MATRIX_SIZE] = { {9,8,7}, {6,5,4}, {3,1,2} };
 int res [MATRIX_SIZE][MATRIX_SIZE];
 for (i = 0; i < MATRIX_SIZE; i++) {
 for (j = 0; j < MATRIX_SIZE; j++) {
 res [i][j] = mat1[i][j] * mat2[j][i];
 }
 }
 }
 **************************************************************************/
