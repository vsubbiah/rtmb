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


public class TestScores {
	private String testScoreName;
	private double weightedThroughput;
	private double minThroughput;
	private double maxThroughput;
	private double meanThroughput;
	private double determinismScore;
	private double throughputScoreForTest;
	private TestResults[]  tResult;
	
	/* 
	 * Are we computing consolidatedMean 
	 * (cm) or is it the geometric mean (gm). 
	 * true  - ConsolidatedMean.
	 * false - GeometricMean.
	 */
	private boolean consolidatedOrGeometricMean;
	
	public TestScores () {
		weightedThroughput = 0;
		minThroughput = 0;
		maxThroughput = 0;
		meanThroughput = 0;
		determinismScore = 0;
		throughputScoreForTest = 0;
		consolidatedOrGeometricMean = true;
	}
	
	public TestScores (TestResults [] tResults) {
		weightedThroughput = 0;
		minThroughput = 0;
		maxThroughput = 0;
		meanThroughput = 0;
		determinismScore = 0;
		throughputScoreForTest = 0;
		consolidatedOrGeometricMean = true;
		this.tResult = tResults;
	}
	
	public String getTestScoreName() {
		return testScoreName;
	}

	public void setTestScoreName(String testScoreName) {
		this.testScoreName = testScoreName;
	}

	public TestResults[] getTestResults() {
		return tResult;
	}

	public void setTestResults (TestResults[] result) {
		tResult = result;
	}

	public double getDeterminismScore() {
		return determinismScore;
	}

	public void setDeterminismScore(double determinismScore) {
		this.determinismScore = determinismScore;
	}

	public double getMaxThroughput() {
		return maxThroughput;
	}

	public void setMaxThroughput(double maxThroughput) {
		this.maxThroughput = maxThroughput;
	}

	public double getMeanThroughput() {
		return meanThroughput;
	}

	public void setMeanThroughput(double meanThroughput) {
		this.meanThroughput = meanThroughput;
	}

	public double getMinThroughput() {
		return minThroughput;
	}

	public void setMinThroughput(double minThroughput) {
		this.minThroughput = minThroughput;
	}

	public double getWeightedThroughput() {
		return weightedThroughput;
	}

	public void setWeightedThroughput(double weightedThroughput) {
		this.weightedThroughput = weightedThroughput;
	}
	public double getThroughputScoreForTest() {
		return throughputScoreForTest;
	}

	public void setThroughputScoreForTest(double tpScoreForTest) {
		this.throughputScoreForTest = tpScoreForTest;
	}

	public boolean isConsolidatedOrGeometricMean() {
		return consolidatedOrGeometricMean;
	}

	public void setConsolidatedOrGeometricMean(boolean consolidatedOrGeometricMean) {
		this.consolidatedOrGeometricMean = consolidatedOrGeometricMean;
	}
}
