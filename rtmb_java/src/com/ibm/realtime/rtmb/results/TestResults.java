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

import com.ibm.realtime.rtmb.params.RealTimeValues;
import com.ibm.realtime.rtmb.params.TestParams;
import com.ibm.realtime.rtmb.tests.util.TestId;

public class TestResults {
	private TestId testId;
	private String testResultName;

	private TestStatistics testStats;

	private int entries;
	private int workQuantum;
	private double [] values;
	private boolean computeMedian;

	public TestResults(TestId testId, int iterations, boolean computeMedian) {
		this.testId = testId;
		this.testStats = new TestStatistics();
		this.entries = 0;
		this.values = new double[iterations];
		this.computeMedian = computeMedian;
	}

	public String getTestResultName() {
		return testResultName;
	}

	/*
	 * copies the fields of arg to this object.
	 */
	public void cloneValues(TestResults t) {
		this.testId = t.testId;
		this.testStats = TestStatistics.cloneValues(t.testStats);
		this.entries = t.entries;
		this.workQuantum = t.workQuantum;
		double values[] = t.getValues();
		
		for( int i=0; i < entries; i++ ) {
			this.values[i] = values[i];
		}
		
		this.testResultName = t.testResultName;
	}

	public void setTestResultName(String testResultName) {
		this.testResultName = testResultName;
	}

	/*
	 * resetResults could be run from within a scope so ensure it does not allocate any
	 * objects but instead clears out the existing objects
	 */
	public void resetResults() {
		this.testStats.resetResults();
		this.entries = 0;
	}
	
	public int getWorkQuantum() {
		return workQuantum;
	}
	
	public void setWorkQuantum(int workQuantum) {
		this.workQuantum = workQuantum;
	}

	public TestId getTestId() {
		return testId;
	}

	public double[] getValues() {
		return values;
	}

	public void setValuesAtNextIndex(double val) {
		this.values[entries] = val;
		testStats.setMinMaxValues(val);
		this.entries++;
		if (this.entries == this.values.length) {
			testStats.compute(values, computeMedian);
		}
	}

	public synchronized void lockedSetValuesAtNextIndex(double val) {
		setValuesAtNextIndex(val);
	}

	public boolean checkStdDev(RealTimeValues rtValues, TestParams params) {
		return testStats.checkStdDev(rtValues, params, values, entries);
	}

	public double getMax() {
		return testStats.getMax();
	}
	
	public double getMin() {
		return testStats.getMin();
	}
	
	public double getStdDev() {
		return testStats.getStdDev();
	}
	
	public double getMedian() {
		return testStats.getMedian();
	}
	
	public double getMean() {
		return testStats.getMean();
	}

	public int getNonConformingIterations() {
		return testStats.getNonConforming();
	}
}
