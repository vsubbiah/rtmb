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

package com.ibm.realtime.rtmb.tools;

import java.io.IOException;
import java.io.InputStream;
import java.io.PrintStream;
import java.text.DecimalFormat;
import java.text.NumberFormat;
import java.util.Arrays;

public class Histogram extends RTMBTool {

	private PrintStream _output;
	private final static NumberFormat _decimalFormatter = new DecimalFormat("#0.000");
	private final static int _buckets = 50;

	public Histogram(InputStream in, PrintStream out) {
		super(in);
		_output = out;
	}

	/**
	 * @param Provide an input file of execution times from the 
	 * Real Time Micro Benchmark Suite, and this will generate 
	 * a file suitable for input to a plotting program like the
	 * Linux graph program.
	 */
	public static void main(String[] args) {
		if (args.length != 0) {
			System.err.println("Syntax: Histogram <report-file >plot-file");
			System.err.println("  where report-file is a report generated by the C, C++ or Java Real Time Micro Benchmark suite");
			System.err.println("  and plot-file is the file to be created");
			return;
		}
		
		Histogram h = new Histogram(System.in, System.out);
		Double[] d = null;
		try {
			d = h.read();
		} catch (IOException e) {
			// TODO Auto-generated catch block
			e.printStackTrace();
		}
		h.write(d);
	}

	private void write(Double[] data) {
		Arrays.sort(data);
		double smallest = data[0];
		double largest = data[data.length-1];
		
		double bucketSize = (largest - smallest)/_buckets;
		
		double prev = smallest;
		double next = prev + bucketSize;
		int count = 0;
		for (int i=0; i<data.length; ++i) {
			if (data[i] < next) {
				++count;
			} else {
				writeEntry(prev, next, count);
				count = 0;
				prev = next;
				next = prev + bucketSize;
				while (data[i] > next) {
					writeEntry(prev, next, count);					
					prev = next;
					next = prev + bucketSize;
				}
				count = 1;
			}
		}
		writeEntry(prev, next, count);
	}

	private void writeEntry(double start, double end, int count) {
		++count; // make the smallest count be 1 for a bucket so that logarithmic plotting will be ok
		String sx =_decimalFormatter.format(start);
		String ex = _decimalFormatter.format(end);
		String sy = "1";
		String ey = Integer.toString(count);
		
		_output.println(sx + " " + sy + " " + 
				sx + " " + ey + " " +
				ex + " " + ey + " " +
				ex + " " + sy);
	}
}
