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

import java.util.Random;

import com.ibm.realtime.rtmb.util.NoPreCompileAnnotation;

@NoPreCompileAnnotation
public class MatrixOperationsWithPrimitives {
	private int size;
	private int matrix1[][];
	private int matrix2[][];
	private int result[][];
	private static final long seed = 1024 * 1024 * 100;
	
	public MatrixOperationsWithPrimitives (int size) {
		Random random = new Random(seed);
		this.size = size;

		this.matrix1 = new int [size][size];
		this.matrix2 = new int [size][size];
		this.result = new int [size][size];

		for (int i = 0; i < size; i++) {
			for (int j = 0; j < size; j++) {
				matrix1[i][j] = random.nextInt(100);
				matrix2[i][j] = random.nextInt(100);
			}
		}
	}

	public int[][] matrixMultiplication() {
		return matrixMultiplicationImpl();
	}

	private int[][] matrixMultiplicationImpl() {
		for(int i = 0; i < size; i++) {
			for(int j = 0; j < size; j++) {
				result[i][j] = 0;
				for(int k = 0; k < size; k++) {
					result[i][j]+= matrix1[i][k] * matrix2[k][j];
				}
			}
		}
		return result;
	}
	public int[][] getResults() {
		return result;
	}
}
