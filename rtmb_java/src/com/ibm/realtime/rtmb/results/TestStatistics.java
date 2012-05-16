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
 
package com.ibm.realtime.rtmb.results;

import java.util.Arrays;

import com.ibm.realtime.rtmb.params.RealTimeValues;
import com.ibm.realtime.rtmb.params.TestParams;
import com.ibm.realtime.rtmb.util.VerbosePrint;

// TestStatistics is a package private class that is used by TestResults
// to provide uniform statistics on a companion array of data
class TestStatistics {
	TestStatistics() {
		init();
	}
	
	public void resetResults() {
		init();
	}
	
	private void init() {
		this.mean = 0;
		this.min = 0;
		this.max = 0;
		this.stdDev = 0;
		this.minUpdated = false;
		this.nonConforming = 0;
	}

	static final TestStatistics cloneValues(TestStatistics t) {
		TestStatistics newTest = new TestStatistics();
		
		newTest.mean = t.mean;
		newTest.median = t.median;
		newTest.min = t.min;
		newTest.max = t.max;
		newTest.stdDev = t.stdDev;
		newTest.minUpdated = t.minUpdated;	
		newTest.nonConforming = t.nonConforming;
		
		return newTest;
	}
	
	double getMedian() {
		return median;
	}

	void setMedian(double[] values) {
		Arrays.sort(values);
		if (values.length % 2 == 0) {
			int pos1 = (values.length / 2) - 1;
			int pos2 = values.length / 2;
			this.median = ((values[pos1] + values[pos2])) / 2;
		} else {
			int pos = ((values.length + 1) / 2) - 1;
			this.median = values[pos];
		}
	}

	double getMax() {
		return max;
	}
	
	void setMax(double cur) {
		if (cur > max) {
			this.max = cur;
		}
	}

	double getMean() {
		return mean;
	}

	void setMean(double values[]) {
		double sum=0;
		for(int i = 0 ; i < values.length ; i++ )  {
			sum += values[i];
		}
		mean = sum/values.length;
	}

	double getMin() {
		return min;
	}
	
	void setMin(double cur) {
		if (!minUpdated) {
			this.min = cur;
			minUpdated = true;
		} else {
			if (cur < min) {
				this.min = cur;
			}
		}
	}
	
	double getStdDev() {
		return stdDev;
	}

	void setStdDev(double values[]) {
		double diff, sqDiff, totalSqDiff = 0;
		double meanTotalSqDiff = 0;

		for (int i = 0; i < values.length; i++) {
			diff = values[i] - mean;
			sqDiff = diff * diff;
			totalSqDiff = totalSqDiff + sqDiff;
		}
		meanTotalSqDiff = totalSqDiff / values.length;
		this.stdDev = Math.sqrt(meanTotalSqDiff);
	}


	int getNonConforming() {
		return nonConforming;
	}
	
	void setMinMaxValues(double cur) {
		setMax(cur);
		setMin(cur);
	}
	
	void compute(double[] values, boolean computeMedian) {
		setMean(values);
		if (computeMedian) {
			setMedian(values);
		}
		setStdDev(values);
	}

	boolean checkStdDev(RealTimeValues rtValues, TestParams tParams, double values[], int entries) {
		int minAcceptableIterations = ( (int) ((rtValues.getAcceptablePercent() / 100) * (entries)) );
		synchronized (this) {
			
			double acceptableDelta = ((rtValues.getRangePercent() / 100) * (this.median));
			double negativeLimit = ( this.median ) - acceptableDelta;
			double positiveLimit = ( this.median ) + acceptableDelta;
			int nonConformingIterations = 0;
			
			for (int i = 0; i < values.length; i++) {
				if ( ( values[i] < negativeLimit) ||
						( values[i] > positiveLimit) ) {
					nonConformingIterations++;
				}
			}

			this.nonConforming = nonConformingIterations;
			print(rtValues, tParams, entries, negativeLimit, minAcceptableIterations, positiveLimit);
			
			if ((entries - nonConformingIterations) < minAcceptableIterations) {
				return false;
			} else {
				return true;
			}
		}
	}

	private void print(RealTimeValues rtValues, TestParams tParams, int entries, double negativeLimit, 
			int minAcceptableIterations, double positiveLimit) {
		VerbosePrint.outPrintln (System.out, tParams, 1);
		VerbosePrint.streamPrint (System.out, tParams, 1, "Measured median for this test run: ");
		VerbosePrint.streamPrintf (System.out, tParams, 1, "%.3f us"  + VerbosePrint.getLineSeparator(), 
				VerbosePrint.roundToMicroSec(median));
		VerbosePrint.streamPrint (System.out, tParams, 1, "Measured maximum time for this test run: ");
		VerbosePrint.streamPrintf (System.out, tParams, 1, "%.3f us"  + VerbosePrint.getLineSeparator(), 
				VerbosePrint.roundToMicroSec(max));
		VerbosePrint.streamPrint (System.out, tParams, 1, "Measured minimum time for this test run: ");
		VerbosePrint.streamPrintf (System.out, tParams, 1, "%.3f us" + VerbosePrint.getLineSeparator(), 
				VerbosePrint.roundToMicroSec(min));
		VerbosePrint.streamPrint (System.out, tParams, 1,"Measured mean for this test run: ");
		VerbosePrint.streamPrintf (System.out, tParams, 1, "%.3f us"  + VerbosePrint.getLineSeparator(), 
				VerbosePrint.roundToMicroSec (mean));
		VerbosePrint.streamPrint (System.out, tParams, 1, "Measured standard deviation for this test run: ");
		VerbosePrint.streamPrintf(System.out, tParams, 1, "%.3f us"  + VerbosePrint.getLineSeparator(), 
				VerbosePrint.roundToMicroSec(stdDev));
		VerbosePrint.streamPrintf(System.out, tParams, 1, "Expected number of iterations between %.3f us and ", 
				VerbosePrint.roundToMicroSec(negativeLimit));
		VerbosePrint.streamPrintf (System.out, tParams, 1, "%.3f us: " + minAcceptableIterations, 
				VerbosePrint.roundToMicroSec(positiveLimit));
		VerbosePrint.outPrintln (System.out, tParams, 1);
		VerbosePrint.streamPrintln (System.out, tParams, 1,	"Total number of conforming iterations: " +
				(entries - this.nonConforming));
		VerbosePrint.streamPrintln (System.out, tParams, 1, "Total number of outlying iterations: " +
				this.nonConforming);
		VerbosePrint.outPrintln (System.out, tParams, 1);
	}
	
	private double mean;
	private double median;
	private double min;
	private double max;
	private double stdDev;
	private int nonConforming;
	
	private boolean minUpdated;

}
