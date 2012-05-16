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

public class MatrixOperationsWithObjects {
	private int rows;
	private int columns;
	private Integer matrix1[][];
	private Integer matrix2[][];

	public MatrixOperationsWithObjects (int size) {
		this.rows = size;
		this.columns = size;
		this.matrix1 = new Integer[rows][columns];
		this.matrix2 = new Integer[rows][columns];
		Random random = new Random();
		for (int i = 0; i < rows; i++) {
			for (int j = 0; j < columns; j++) {
				matrix1[i][j] = Integer.valueOf(random.nextInt(100));
				matrix2[i][j] = Integer.valueOf(random.nextInt(100));
			}
		}
	}
	
	public void mathOnMatrices() {
		matrixMultiplicationImpl();
		sumAllNumbersInMatrix();
	}
	
	public void matrixMultiplication() {
		matrixMultiplicationImpl();
	}
		
	private void matrixMultiplicationImpl() {
		Integer res[][] = new Integer[rows][columns];
		for(int i = 0; i < rows; i++) {
			for(int j = 0; j < columns; j++) {
				res[i][j] = 0;
				for(int k = 0; k < columns; k++) {
					res[i][j]+= Integer.valueOf(matrix1[i][k].intValue() * matrix2[k][j].intValue());
				}	
			}			
		}
	}
	
	private void sumAllNumbersInMatrix() {
		Integer res[] = new Integer[rows * columns];
		int k = 0;
		for (int i = 0; i < rows; i++) {
			for (int j = 0; j < columns; j++, k++) {
				res[k] = matrix1[i][j] + matrix2[j][i];
			}
		}
	}
}
