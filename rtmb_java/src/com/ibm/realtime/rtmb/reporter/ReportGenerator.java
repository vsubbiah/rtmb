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
 
package com.ibm.realtime.rtmb.reporter;

import java.io.File;
import java.io.FileNotFoundException;
import java.io.IOException;
import java.io.PrintStream;
import java.text.SimpleDateFormat;
import java.util.Date;
import java.util.Enumeration;

import com.ibm.realtime.rtmb.RTMB;
import com.ibm.realtime.rtmb.params.TestInfo;
import com.ibm.realtime.rtmb.params.TestParams;
import com.ibm.realtime.rtmb.results.FinalMicroBenchmarkTestResult;
import com.ibm.realtime.rtmb.results.ResultTable;
import com.ibm.realtime.rtmb.results.TestResults;
import com.ibm.realtime.rtmb.results.TestScores;
import com.ibm.realtime.rtmb.util.VerbosePrint;

public class ReportGenerator {
	private RTMB rtmbMain; 
	private File reportDir;
	private TestParams tparams;

	public ReportGenerator(RTMB rtmb, TestParams params) {
		setupReportDirectory(params);
		this.tparams = params;
		this.rtmbMain = rtmb;
	}

	private void setupReportDirectory(TestParams params) {
		SimpleDateFormat df = new SimpleDateFormat("yyyyMMdd'_'hhmmssSS");
		String date = df.format(new Date());
		/*
		 * Create a separate folder under report dir.
		 */
		String completeReportDirName = params.getReportDirName() + VerbosePrint.getFileSeparator() + "java_report_" + date; 
		this.reportDir = new File (completeReportDirName);
		if (!this.reportDir.mkdirs()) {
			System.err.println("Internal Error: Unable to create report directory: " + reportDir.getAbsolutePath());
			return;
		}
	}

	public void generateReportForAllTests(ResultTable rTable) {
		/*
		 * Walk the hashtable
		 */
		Enumeration<TestInfo> eKeys = ResultTable.getResults().keys();
		File systemReportFile = new File (reportDir.getPath() + VerbosePrint.getFileSeparator() + "SystemReport.txt");
		try {
			systemReportFile.createNewFile();
		} catch (IOException e1) {
			e1.printStackTrace();
			return;
		}
		PrintStream systemReportFileStream = null;
		try {
			systemReportFileStream = new PrintStream (systemReportFile);
		} catch (FileNotFoundException e1) {
			e1.printStackTrace();
			return;
		}
		VerbosePrint.streamPrint(systemReportFileStream, "Final throughput score for the system: ");
		VerbosePrint.streamPrintf (systemReportFileStream, "%.3f" + VerbosePrint.getLineSeparator(), rtmbMain.getResults().getThroughputScoreForSystem());
		VerbosePrint.streamPrint (systemReportFileStream, "Final consistency score for the system: ");
		VerbosePrint.streamPrintf (systemReportFileStream, "%.3f" + VerbosePrint.getLineSeparator(), rtmbMain.getResults().getConsistencyScoreForSystem());
		while( eKeys.hasMoreElements() ) {
			TestInfo t = eKeys.nextElement();
			File testReportDir = new File (reportDir.getPath() + VerbosePrint.getFileSeparator() + t.getTestName());
			if (!testReportDir.mkdir()) {
				System.err.println("Internal Error: Unable to create report directory: " + testReportDir.getAbsolutePath());
				return;
			}
			File summaryReport = new File (testReportDir.getPath() + VerbosePrint.getFileSeparator() + "report.txt");
			PrintStream pStream = null;
			try {
				summaryReport.createNewFile();
				pStream = new PrintStream (summaryReport);
			} catch (IOException e) {
				e.printStackTrace();
				return;
			}
			VerbosePrint.streamPrintln(pStream, "Report for test: " + t.getTestName());
			VerbosePrint.streamPrintln(pStream, "==================================");
			FinalMicroBenchmarkTestResult fbtResults = rTable.getFinalTestResultsByTestInfo(t);
			if (fbtResults.getOverallConsistencyScore() != -1) {
				VerbosePrint.streamPrint(pStream, "Score for determinism: ");
				VerbosePrint.streamPrintf (pStream, "%.3f" + 
						VerbosePrint.getLineSeparator(), fbtResults.getOverallConsistencyScore());
			}
			VerbosePrint.streamPrint(pStream, "Score for throughput: ");
			VerbosePrint.streamPrintf (pStream, "%.3f" + 
					VerbosePrint.getLineSeparator(), fbtResults.getOverallTestScore());
			VerbosePrint.streamPrintln(pStream);
			TestScores [] tScores = fbtResults.getTestScores();
			for (int j = 0; j < tScores.length; j++) {
				if (tScores.length > 1) {
					VerbosePrint.streamPrintln(pStream);
					VerbosePrint.streamPrintln(pStream, "Scores for " + "subtest: "+ tScores[j].getTestScoreName());
					VerbosePrint.streamPrintln(pStream, "============================================");
				}
				VerbosePrint.streamPrint(pStream, "Maximum throughput score   : "); 
				VerbosePrint.streamPrintf (pStream, "%.3f" + 
						VerbosePrint.getLineSeparator(), tScores[j].getMaxThroughput());
				VerbosePrint.streamPrint(pStream, "Minimum throughput score   : "); 
				VerbosePrint.streamPrintf (pStream, "%.3f" + 
						VerbosePrint.getLineSeparator(), tScores[j].getMinThroughput());
				VerbosePrint.streamPrint(pStream, "Mean throughput score      : ");  
				VerbosePrint.streamPrintf (pStream, "%.3f" + 
						VerbosePrint.getLineSeparator(), tScores[j].getMeanThroughput());
				if (tScores[j].getWeightedThroughput() != -1) {
					VerbosePrint.streamPrint(pStream, "Weighted throughput score  : "); 
					VerbosePrint.streamPrintf (pStream, "%.3f" + 
							VerbosePrint.getLineSeparator(),tScores[j].getWeightedThroughput());
				}
				if (tScores.length > 1 && (tScores[j].getDeterminismScore() != -1)) {
					VerbosePrint.streamPrint(pStream, "Determinism score          : ");
					VerbosePrint.streamPrintf (pStream, "%.3f" + 
							VerbosePrint.getLineSeparator(),tScores[j].getDeterminismScore());
				}
				TestResults [] tResults = tScores[j].getTestResults();
				for (int i = 0; i < tResults.length; i++) {
					VerbosePrint.streamPrintln(pStream);
					if ( tResults.length > 1 ) {
						VerbosePrint.streamPrintln(pStream, "Test results for: " + tResults[i].getTestResultName());						
						VerbosePrint.streamPrintln(pStream, "============================================");
					}
					VerbosePrint.streamPrintln(pStream, "Total number of iterations " +
							"of test run: " + t.getTestIterations());					
					VerbosePrint.streamPrint(pStream, "Measured median for this test run: ");
					VerbosePrint.streamPrintf (pStream, "%.3f us" + VerbosePrint.getLineSeparator(), 
							VerbosePrint.roundToMicroSec(tResults[i].getMedian()));

					VerbosePrint.streamPrint(pStream, "Measured maximum time for this test run: ");
					VerbosePrint.streamPrintf (pStream, "%.3f us" + VerbosePrint.getLineSeparator(), 
							VerbosePrint.roundToMicroSec(tResults[i].getMax()));

					VerbosePrint.streamPrint(pStream, "Measured minimum time for this test run ");
					VerbosePrint.streamPrintf (pStream, "%.3f us" + VerbosePrint.getLineSeparator(), 
							VerbosePrint.roundToMicroSec(tResults[i].getMin()));

					VerbosePrint.streamPrint(pStream, "Measured mean for this test run ");
					VerbosePrint.streamPrintf (pStream, "%.3f us" + VerbosePrint.getLineSeparator(),
							VerbosePrint.roundToMicroSec(tResults[i].getMean()));

					VerbosePrint.streamPrint(pStream, "Measured standard deviation for this test run: ");
					VerbosePrint.streamPrintf (pStream, "%.3f us" + VerbosePrint.getLineSeparator(), 
							VerbosePrint.roundToMicroSec(tResults[i].getStdDev()));

					VerbosePrint.streamPrintln(pStream, "Total number of conforming " +
							"iterations: " + (tResults[i].getValues().length - tResults[i].getNonConformingIterations()));
					VerbosePrint.streamPrintln(pStream, "Total number of outlying iterations: " + 
							tResults[i].getNonConformingIterations());

					//Measured execution times in the report should show up for -v:simple					
					if (this.tparams.getVerboseLevel() >= 1) {
						double [] values = tResults[i].getValues();
						VerbosePrint.streamPrintln(pStream, "Measured execution times:");
						for (int k = 0; k < values.length; k++) {
							VerbosePrint.streamPrintln(pStream, "Execution time " + k + ": " + values[k]);	
						}
					}
				}
			}
		}
	}
}
