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
 
#include <math_opern.h>

/*
 * Name    : do_simple_int_opern()
 * Desc    : Does simple integral arithmetic.
 * Args    : num - int - maximum loops
 * Returns : res - result of the arithmetic.
 */
int do_simple_int_opern(int num) {
	long int i, j;
	long int res = 0;
	for (i = 2, j = 10000; i < num; i++, j -= i) {
		res = ((j * i) - (j + i)) / j;
	}
	return res;
}

/*
 * Name    : do_simple_float_opern()
 * Desc    : Does simple floating point arithmetic.
 * Args    : num - double - maximum loops
 * Returns : res - result of the arithmetic.
 */
double do_simple_float_opern(int num) {
	double i, j, k;
	double res = 0;
	for (i = 2, j = 10000; i < num; i++, j -= i) {
		k = j / i;
		res = ((j * k) / (j - i)) / i;
	}
	return res;
}

/*
 * Name    : add_n_numbers()
 * Desc    : Adds numbers.
 * Args    : num - Total numbers for addition.
 * Returns : tot - result of the arithmetic.
 */
long long add_n_numbers(int num) {
	int i;
	long long tot = 0;
	for (i = 0; i < num; i++) {
		tot += (tot % num);
	}
	return tot;
}
