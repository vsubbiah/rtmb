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

import java.util.Arrays;

import com.ibm.realtime.rtmb.params.RealTimeValues;
import com.ibm.realtime.rtmb.params.TestInfo;
import com.ibm.realtime.rtmb.params.TestParams;
import com.ibm.realtime.rtmb.results.TestResults;
import com.ibm.realtime.rtmb.results.TestScores;

public abstract class SpecTest {
	protected final static int scale = 1000000;
	public abstract String name();
	
	public abstract TestResults[] benchMarkTest(TestInfo tInfo, RealTimeValues rtValues,
			TestParams tParams) throws NotEligibleException, TestIncompleteException;
	/**
	 * generateCombinedScore: This is the default implementation of
	 * score generator. Tests can override the generation of combined score.
	 */
	public TestScores [] generateCombinedScore (TestInfo tInfo, TestResults [] tResult) {
		double contributionPercent = 0;
		double combinedThroughput = 0;
		double minThroughput = 0;
		double maxThroughput = 0;
		double sumThroughput = 0;
		double consistencySum = 0;
		double median = 0;
		int k = 0;

		double [] tmpArray = new double[tResult.length];
		for(k = 0;  k < tResult.length; k++){
			tmpArray[k] += tResult[k].getMedian();
		}
		Arrays.sort(tmpArray);
		if (tmpArray.length % 2 == 0) {
			int pos1 = (tmpArray.length / 2) - 1;
			int pos2 = tmpArray.length / 2;
			median = (tmpArray[pos1] + tmpArray[pos2]) / 2;
		} else {
			int pos = ((tmpArray.length + 1) / 2) - 1;
			median = tmpArray[pos];
		}
		
		double values[] = tResult[0].getValues();
		maxThroughput = scale * 1.0 / time(values[0]);
		minThroughput = scale * 1.0 / time(values[0]);

		for (int i = 0; i < tResult.length; i++) {
			values = tResult[i].getValues();

			for (int j = 0; j < values.length; j++) {

				/* Throughput calculation is here */
				double value = time(values[j]);
				double throughput = 1.0 * scale / value;

				if (throughput < minThroughput) {
					minThroughput = throughput;
				} else if (throughput > maxThroughput) {
					maxThroughput = throughput;
				}
				sumThroughput += throughput;
				contributionPercent = getContribution(value, median); 
				combinedThroughput += (contributionPercent * scale) / (value);

				/* consistency calculation is here */
				consistencySum += contributionPercent;
			}
		}
		TestScores [] tScores = new TestScores[1];
		tScores[0] = new TestScores();
		tScores[0].setDeterminismScore(consistencySum / (tResult.length * values.length));
		tScores[0].setMaxThroughput(maxThroughput);
		tScores[0].setMinThroughput(minThroughput);
		tScores[0].setMeanThroughput(sumThroughput / (tResult.length * values.length));
		tScores[0].setWeightedThroughput( combinedThroughput / (100.0 * (tResult.length * values.length)) );
		tScores[0].setTestResults(tResult);
		tScores[0].setThroughputScoreForTest(tInfo.getReferenceScore() / median);
		return tScores;
	}
	
	private double time(double value) {
		/* We ensure that throughput is never infinite by rounding up a time to at least 1ns */		
		if (value < 1.0 && value >= 0.0) {
			value = 1.0;
		}
		return value;
	}
	
	protected int updateIterations(TestInfo tInfo, RealTimeValues rtValues, TestParams tParams) {
		int newIters;
		if (tParams.isQuickRun()) {
			newIters = 1;
		} else {
			newIters = tInfo.getTestIterations() * rtValues.getMultiplier();
		}
		tInfo.setTestIterations(newIters);
		
		return newIters;
	}
	
	protected TestResults[] establishSingleResult(TestId testId, int testIterations) {
		/*
		 * Create the TestResult[] object. This test generates
		 * just one set of results. So, the TestResult[] array
		 * will just have one element in it.
		 */
		TestResults[] testResult = new TestResults[1];
		testResult[0] = new TestResults(testId, testIterations, true);
		
		return testResult;
	}

	private static final double eps = 0.000001;
	private static double getContribution(double value, double median) {
		double percent = 0;
		double diff = value - median;

		if (Math.abs(diff) < eps) {
			percent = 100.0;
		} else if (diff + median < eps) {
			percent = 10.0;
		} else if (Math.abs(diff - median) < eps) {
			percent = 0.0;
		} else  {
			/* take absolute value */
			if (diff < 0.0)
				diff = -diff;

			if (diff > median) {
				percent = 0.0;
			} else {
				percent = value * 100.0 / median;

				if (percent > 200.0)
					percent = 0.0;
				else if (Math.abs(percent - 100.0) < eps){
					percent = 100.0;
				}
				else if( percent < 100.0) {
					percent =  (((percent + 10.0)) / 10.0);
					percent *= 10.0;
				}
				else {
					double difference = percent - 100.0;
					percent = 100.0 - difference;
				}
			}
		}
		return percent;
	}
}
