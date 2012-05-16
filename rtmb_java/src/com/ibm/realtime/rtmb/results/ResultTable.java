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

import java.util.Collection;
import java.util.Enumeration;
import java.util.Hashtable;
import java.util.Iterator;

import com.ibm.realtime.rtmb.RTMB;
import com.ibm.realtime.rtmb.params.TestInfo;
import com.ibm.realtime.rtmb.tests.util.TestId;

public class ResultTable {
	private static ResultTable instance = new ResultTable();
	private static Hashtable<TestInfo, FinalMicroBenchmarkTestResult> results = new Hashtable<TestInfo, FinalMicroBenchmarkTestResult>();
	private double throughputScoreForSystem;
	private double consistencyScoreForSystem;
	private double finalScoreForSystem;

	public static ResultTable getInstance() {
		return instance;
	}

	public void addResults(TestInfo test, FinalMicroBenchmarkTestResult res) {
		results.put(test, res);
	}

	public TestResults[] getTestResultsByTestInfo(TestInfo tInfo) {
		FinalMicroBenchmarkTestResult ftr = results.get(tInfo);
		return ftr.getTestScores()[0].getTestResults();
	}

	public TestResults[] getTestResultsByTestID(TestId testId) {
		Collection<FinalMicroBenchmarkTestResult> c = results.values();
		Iterator<FinalMicroBenchmarkTestResult> i = c.iterator();
		while (i.hasNext()) {
			FinalMicroBenchmarkTestResult ftResult = i.next();
			TestScores [] tScores = ftResult.getTestScores();
			TestResults[] tResults = tScores[0].getTestResults();
			if (tResults[0].getTestId() == testId) {
				return tResults;
			}
		}
		return null;
	}

	public FinalMicroBenchmarkTestResult getFinalTestResultsByTestInfo(TestInfo tInfo) {
		return results.get(tInfo);
	}

	public double getThroughputScoreForSystem() {
		return throughputScoreForSystem;
	}

	public double getConsistencyScoreForSystem() {
		return consistencyScoreForSystem;
	}

	public void setThroughputScoreForSystem() {
		Enumeration<TestInfo> eKeys = results.keys();
		double throughputProduct = 1;
		while(eKeys.hasMoreElements()) {
			TestInfo t = eKeys.nextElement();
			FinalMicroBenchmarkTestResult fResult = results.get(t);
			if (fResult.getOverallThroughputScore() > 0) {
				throughputProduct*= Math.pow(fResult.getOverallThroughputScore(), 
						(1.0 / RTMB.getNumberOfExecutedTests()));

			}
		}
		throughputScoreForSystem = throughputProduct;
	}

	public void setConsistencyScoreForSystem() {
		Enumeration<TestInfo> eKeys = results.keys();
		double consistencyProduct = 1;
		while(  eKeys.hasMoreElements()) {
			TestInfo t = eKeys.nextElement();
			FinalMicroBenchmarkTestResult fResult = results.get(t);
			if (fResult.getOverallConsistencyScore() > 0) {
				consistencyProduct*= Math.pow(fResult.getOverallConsistencyScore(), 
						(1.0 / RTMB.getNumberOfExecutedTests()));
			}
		}
		consistencyScoreForSystem = consistencyProduct;
	}

	public static Hashtable<TestInfo, FinalMicroBenchmarkTestResult> getResults() {
		return results;
	}
	
	public void setFinalScoreForSystem() {
		this.finalScoreForSystem = Math.pow(consistencyScoreForSystem * throughputScoreForSystem, 
				(1.0 / 2));
	}

	public double getFinalScoreForSystem() {
		return this.finalScoreForSystem;
	}
}

