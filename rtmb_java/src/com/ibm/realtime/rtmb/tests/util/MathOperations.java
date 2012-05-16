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

public class MathOperations {
	private MathOperations() {
		
	}
	/**
	 * @param num - Total numbers for addition.
	 * @return tot - result of the arithmetic.
	 */
	public static long addNumbers (int numbers) {
		int i;
		long tot = 0;
		for (i = 0; i < numbers; i++) {
			tot+=(tot % numbers);
		}
		return tot;
	}

	/**
	 * do_simple_int_opern()
	 * @param num - maximum loops
	 * @return res - result of the arithmetic.
	 */
	public static int do_simple_int_opern (int num) {
		int i, j;
		int res = 0;
		for (i = 2, j = 10000; i < num; i++, j-=i) {
			res = ((j * i) - (j + i))/j;
		}
		return res;
	}

	/**
	 * do_simple_float_opern()
	 * @param num - double - maximum loops
	 * @return res - result of the arithmetic.
	 */
	public static double do_simple_float_opern (int num) {
		double i, j, k;
		double res = 0;
		for (i = 2, j = 10000; i < num; i++, j-=i) {
			k = j / i;
			res = ((j * k) / (j - i)) / i;
		}
		return res;
	}

}
