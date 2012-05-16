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

public class Matrix{
	private int size;
	private int data[][];

	public Matrix(int size) {
		this.size = size;

		this.data = new int [size][size];
	}

	public Matrix(int size,boolean initialize) {
		this.size = size;
		this.data = new int [size][size];

		randomFill( data , size, 1000 ) ;
	}

	public static void randomFill(int[][] data,int size, long seed) {
		Random random = new Random(seed);
		
		for (int i = 0; i < size; i++) {
			for (int j = 0; j < size; j++) {
				data[i][j] = random.nextInt(100);
			}
		}
	}

	public void multiply(Matrix a, Matrix b) {
		int data1[][] = a.getData();
		int data2[][] = b.getData();

		multiply( data1 , data2 , this.data, size) ;
	}

	public static void multiply( int[][] data1 ,int[][] data2, int[][] data3, int size)  {
		int sum =0;

		for(int i = 0; i < size; i++) {
			for(int j = 0; j < size; j++) {
				sum = 0;
				
				for(int k = 0; k < size; k++) {
					sum += data1[i][k] * data2[k][j];
				}

				data3[i][j] = sum;
			}
		}

		return ;
	}

	public int[][] getData() {
		return data;
	}
}
