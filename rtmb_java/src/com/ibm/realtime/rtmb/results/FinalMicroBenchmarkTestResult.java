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

public class FinalMicroBenchmarkTestResult {
	private TestScores [] tScores;
	private double overallTestScore;
	private double overallThroughputScore;
	private double overallConsistencyScore;

	public FinalMicroBenchmarkTestResult (TestScores [] tScores) {
		this.tScores = tScores;
		this.overallConsistencyScore = -1;
		this.overallThroughputScore = -1;
		this.overallTestScore = -1;
	}

	public TestScores[] getTestScores() {
		return tScores;
	}
	
	public double getOverallTestScore() {
		return overallTestScore;
	}
	public void setOverallTestScore(double overallTestScore) {
		this.overallTestScore = overallTestScore;
	}

	public double getOverallConsistencyScore() {
		return overallConsistencyScore;
	}
	
	public double getOverallThroughputScore() {
		return overallThroughputScore;
	}

	public void setOverallConsistencyScore(double overallConsistencyScore) {
		this.overallConsistencyScore = overallConsistencyScore;
	}

	public void setOverallThroughputScore(double productOfScores) {
		this.overallThroughputScore = productOfScores;
	}
	
}

