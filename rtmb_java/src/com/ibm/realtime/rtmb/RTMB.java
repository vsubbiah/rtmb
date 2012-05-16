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
 
package com.ibm.realtime.rtmb;

import java.io.FileInputStream;
import java.io.FileNotFoundException;
import java.io.IOException;
import java.util.Iterator;
import java.util.LinkedList;
import java.util.jar.JarInputStream;
import java.util.zip.ZipEntry;

import com.ibm.realtime.rtmb.params.InvalidInputFileFormatException;
import com.ibm.realtime.rtmb.params.TestConfig;
import com.ibm.realtime.rtmb.params.TestInfo;
import com.ibm.realtime.rtmb.params.TestParams;
import com.ibm.realtime.rtmb.reporter.ReportGenerator;
import com.ibm.realtime.rtmb.results.FinalMicroBenchmarkTestResult;
import com.ibm.realtime.rtmb.results.ResultTable;
import com.ibm.realtime.rtmb.results.TestResults;
import com.ibm.realtime.rtmb.results.TestScores;
import com.ibm.realtime.rtmb.tests.impl.BenchMarkTestFactory;
import com.ibm.realtime.rtmb.tests.impl.BenchMarkThreadFactory;
import com.ibm.realtime.rtmb.tests.util.BenchMarkTestThread;
import com.ibm.realtime.rtmb.tests.util.NotEligibleException;
import com.ibm.realtime.rtmb.tests.util.PriorityType;
import com.ibm.realtime.rtmb.tests.util.RTSJCheck;
import com.ibm.realtime.rtmb.tests.util.SpecTest;
import com.ibm.realtime.rtmb.tests.util.TestIncompleteException;
import com.ibm.realtime.rtmb.util.NoPreCompileAnnotation;
import com.ibm.realtime.rtmb.util.VerbosePrint;

/**
 * Main Java file for RTMB
 */
public class RTMB {
	private TestParams testParams;
	private TestConfig testConfig;
	private BenchMarkTestFactory factory;
	private ResultTable results;
	private static int numberOfExecutedTests = 0;
	
	public static final String benchmarkName() {
		return "rtmb";
	}

	public ResultTable getResults() {
		return results;
	}

	public RTMB(TestParams clParams, TestConfig iflParams) {
		this.factory = BenchMarkTestFactory.getInstance();
		this.results = new ResultTable();
		this.testParams = clParams;
		this.testConfig = iflParams;
	}

	/**
	 * Runs the requested benchmark test.
	 * @param clp The CmdLineParams object.
	 * @param test The corresponding BenchMarkTest object for the test to be run.
	 */
	private void runTest(TestInfo test) {
		SpecTest testToRun;
		try {
			testToRun = factory.getTest(test);
		} catch (ClassNotFoundException e1) {
			VerbosePrint.errPrintln("RTMB: " + test.getTestName() + " is an RTSJ specific micro-benchmark " +
					"and cannot " + VerbosePrint.getLineSeparator() + "be run with a Java implementation that" +
			" is not RTSJ compliant.");
			return;
		}
		TestScores [] scr = null;
		FinalMicroBenchmarkTestResult ftr = null;
		TestResults [] tResult = null;

		BenchMarkThreadFactory thrFactory = BenchMarkThreadFactory.getInstance();
		BenchMarkTestThread tThread = null;

		if (testParams.getRTmode() == 0) {
			tThread = thrFactory.getThread(PriorityType.HighPriority, testToRun,
					test, testConfig.getSoftRealTimeValues(), testParams);
		} else if (testParams.getRTmode() == 1) {
			tThread = thrFactory.getThread(PriorityType.HighPriority, testToRun,
					test, testConfig.getHardRealTimeValues(), testParams);
		} else {
			tThread = thrFactory.getThread(PriorityType.HighPriority, testToRun,
					test, testConfig.getGuaranteedRealTimeValues(), testParams);
		}
		tThread.execute();
		try {
			tThread.waitForThreadExit();
		} catch (InterruptedException e) {
			VerbosePrint.errPrintln ("Test execution Interrupted");
			e.printStackTrace();
			return;
		}
		try {
			tResult = tThread.getTestResults();
			if (tResult != null) {
				scr = testToRun.generateCombinedScore(test, tResult);
				ftr = new FinalMicroBenchmarkTestResult(scr);
				generateOverallScoreForMicroBenchmark (ftr);
			}
		} catch (NotEligibleException e) {
			VerbosePrint.errPrintln ("RTMB ERROR: Test " + test.getTestName() + 
			" is not eligible to run on this system.");
			return;
		} catch (TestIncompleteException e) {
			e.printStackTrace();
			tResult = e.getTestResults();
			scr = new TestScores [1];
			scr[0] = new TestScores(tResult);
			ftr = new FinalMicroBenchmarkTestResult(scr);
		}
		if (ftr != null) {
			printScores(test, ftr);	
			results.addResults(test, ftr);
		}
		incrementNumExecutedTests();
	}

	private void printScores(TestInfo tInfo, FinalMicroBenchmarkTestResult ftr) {
		VerbosePrint.streamPrintln(System.out, testParams, 1, "Scores:");
		TestScores [] tScores = ftr.getTestScores();
		for (int i = 0; i < tScores.length; i++) {
			if (tScores.length > 1) {
				VerbosePrint.outPrintln(System.out, testParams, 1);
				VerbosePrint.streamPrintln(System.out, testParams, 1, "Score for subtest: " +
						tScores[i].getTestScoreName());
				VerbosePrint.outPrintln(System.out, testParams, 1);
			}
			VerbosePrint.streamPrint (System.out, testParams, 1, "Maximum throughput  : ");
			VerbosePrint.streamPrintf (System.out, testParams, 1, "%.3f" + VerbosePrint.getLineSeparator(), tScores[i].getMaxThroughput());
			VerbosePrint.streamPrint(System.out, testParams, 1, "Minimum throughput  : ");
			VerbosePrint.streamPrintf (System.out, testParams, 1, "%.3f" + VerbosePrint.getLineSeparator(), tScores[i].getMinThroughput());
			VerbosePrint.streamPrint(System.out, testParams, 1, "Mean throughput     : ");
			VerbosePrint.streamPrintf (System.out, testParams, 1, "%.3f" + VerbosePrint.getLineSeparator(), tScores[i].getMeanThroughput()); 
			double weightedTP = -1;
			if ((weightedTP = tScores[i].getWeightedThroughput()) >= 0) {
				VerbosePrint.streamPrint (System.out, testParams, 1, "Weighted throughput : ");
				VerbosePrint.streamPrintf (System.out, testParams, 1, "%.3f" + VerbosePrint.getLineSeparator(), weightedTP);
			}
			if (tScores.length > 1) {
				double consPercent = -1;
				if ((consPercent = tScores[i].getDeterminismScore()) >= 0) {
					VerbosePrint.streamPrint (System.out, testParams, 1, "Determinism Score   : ");
					VerbosePrint.streamPrintf (System.out, testParams, 1, "%.3f" + VerbosePrint.getLineSeparator(), consPercent);
				}
			}
		}
		VerbosePrint.outPrintln(System.out, testParams, 1);
		VerbosePrint.streamPrint (System.out, testParams, 1, "Final Throughput Score  : ");
		VerbosePrint.streamPrintf (System.out, testParams, 1, "%.3f" + VerbosePrint.getLineSeparator(), ftr.getOverallThroughputScore());
		VerbosePrint.streamPrint (System.out, testParams, 1, "Final Determinism Score : ");
		VerbosePrint.streamPrintf (System.out, testParams, 1, "%.3f" + VerbosePrint.getLineSeparator(), ftr.getOverallConsistencyScore());
		VerbosePrint.outPrintln(System.out, testParams, 1);
		VerbosePrint.streamPrint (System.out, testParams, 1, "Final Score for " + tInfo.getTestName() + " micro-benchmark: ");
		VerbosePrint.streamPrintf (System.out, testParams, 1, "%.3f" + VerbosePrint.getLineSeparator(), ftr.getOverallTestScore());
		VerbosePrint.outPrintln(System.out, testParams, 1);
	}

	private void generateOverallScoreForMicroBenchmark (FinalMicroBenchmarkTestResult ftr) {
		TestScores [] tScores = ftr.getTestScores();
		double productOfConsistencyPercent= 1;
		double productOfThroughputScores = 1;
		for (int i = 0; i < tScores.length; i++) {
			if (tScores[i].getThroughputScoreForTest() > 0) {
				productOfThroughputScores *= Math.pow(tScores[i].getThroughputScoreForTest(), (1.0 / tScores.length));
			}
			if (tScores[i].getDeterminismScore() > 0) {
				productOfConsistencyPercent *= Math.pow(tScores[i].getDeterminismScore(), (1.0 / tScores.length));
			}
		}
		ftr.setOverallThroughputScore(productOfThroughputScores);
		ftr.setOverallConsistencyScore(productOfConsistencyPercent);
		double overallScore = Math.pow (productOfConsistencyPercent * productOfThroughputScores, (1.0 / 2));
		ftr.setOverallTestScore(overallScore);
	}

	/**
	 * Launches all tests based on command line arguments and input parameters.
	 */

	private void launchTests() {
		boolean found = false;
		LinkedList<String> cmdLineTestNames = testParams.getTestNames();
		Iterator<TestInfo> testInfoIterator = testConfig.getTests().iterator();

		/*
		 * Verify that the user has not provided any config tests to run
		 */
		if (cmdLineTestNames != null) {
			Iterator<String> testnameIterator = cmdLineTestNames.iterator();
			while (testnameIterator.hasNext()) {
				found = false;
				String str = testnameIterator.next();
				while (testInfoIterator.hasNext()) {
					TestInfo test = testInfoIterator.next();
					if (str.equals(test.getTestName())) {
						if (test.isConfigTest()) {
							VerbosePrint.errPrintln("");
							VerbosePrint.errPrintln("RTMB ERROR: Attempting to " +
									"run a config test " + test.getTestName() + " using \"--test/-t\" option");
							testnameIterator.remove();
							return;
						}
					}
				}
			}
		}

		testInfoIterator = testConfig.getTests().iterator();

		/*
		 * Run all the config tests at first
		 */
		VerbosePrint.configStart(System.out, testParams);
		while (testInfoIterator.hasNext()) {
			TestInfo test = testInfoIterator.next();
			if (test.isConfigTest()) {
				runTest(test);
			}
		}
		
		VerbosePrint.microBenchStart(System.out, testParams);		

		testInfoIterator = testConfig.getTests().iterator();
		/*
		 * If no tests names are provided in the cmd line run all the
		 * tests provided in the input file.
		 */
		if (cmdLineTestNames != null) {
			Iterator<String> testnameIterator = cmdLineTestNames.iterator();
			while (testnameIterator.hasNext()) {
				found = false;
				String str = testnameIterator.next();
				while (testInfoIterator.hasNext()) {
					TestInfo test = testInfoIterator.next();
					if (str.equals(test.getTestName())) {
						if (!test.isConfigTest()) {
							found = true;
							runTest(test);
							break;
						} else {
							VerbosePrint.errPrintln();
							VerbosePrint.errPrintln("RTMB ERROR: Attempting to " +
									"run a config test " + test.getTestName() + " using \"--test/-t\" option");
							return;
						}
					}
				}
				testInfoIterator = testConfig.getTests().iterator();
				if (!found) {
					VerbosePrint.errPrintln ("ERROR: " + str + " test " +
							"does not contain an entry in the input file. " +
					"Skipping it...");
				}
			}
		} else {
			for (int j = 0; j < testConfig.getListSize(); j++) {
				TestInfo test = testConfig.getTestEntry(j);
				if (!test.isConfigTest()) {
					runTest(test);
				}
			}
		}
	}

	private static void loadAndCompileClasses() {
		String jarFile = System.getProperty("user.dir") + "/lib/" + RTMB.benchmarkName() + ".jar";
		FileInputStream fis = null;
		JarInputStream jis = null;
		try {
			fis = new FileInputStream(jarFile);
		} catch (FileNotFoundException e) {
			System.err.println("Warning: Benchmark built incorrectly. Java class files need to be packaged into: " + jarFile);
			System.err.println("Results will not be valid");
			return;
		}
		try {
			jis = new JarInputStream(fis);
		} catch (IOException e) {
			e.printStackTrace();
			return;
		}
		ZipEntry zipEntry;
		try {
			while ((zipEntry = jis.getNextEntry()) != null) {
				String classPathName = zipEntry.getName();
				if (classPathName.endsWith(".class")) {
					if (classPathName.contains("RTSJSpecific")) {
						if (!RTSJCheck.checkIfRTSJ()) {
							continue;
						}
					}
					String classPackageName = classPathName.replaceAll("/", ".");
					String className[] = classPackageName.split(".class");
					Class<?> clazz = Class.forName(className[0]);
					if (clazz.isAnnotationPresent(NoPreCompileAnnotation.class)) {
						continue;
					} else {
						Compiler.compileClass(clazz);
					}
				}
			}
			jis.close();
		} catch (IOException e) {
			e.printStackTrace();
		} catch (ClassNotFoundException e) {
			e.printStackTrace();
		}
	}

	public static int getNumberOfExecutedTests() {
		return numberOfExecutedTests;
	}

	private static void incrementNumExecutedTests () {
		numberOfExecutedTests++;
	}

	// main method.
	public static void main(String[] args) {
		loadAndCompileClasses();
		Compiler.disable();
		TestParams clParams = new TestParams(args);
		if (!clParams.ok()) {
			return;
		}
		TestConfig iflParams = null;
		int status  = 0;
		try {
			iflParams = new TestConfig(clParams);
		} catch (InvalidInputFileFormatException iiffe) {
			VerbosePrint.errPrintln ("Exception while parsing " +
			"input file:");
			iiffe.printStackTrace();
			return;
		} catch (IOException ioe) {
			VerbosePrint.errPrintln ("Exception while parsing " +
			"input file:");
			ioe.printStackTrace();
			return;
		}
		RTMB rtmb = new RTMB(clParams, iflParams);
		rtmb.launchTests();
		rtmb.results.setThroughputScoreForSystem();
		rtmb.results.setConsistencyScoreForSystem();
		rtmb.results.setFinalScoreForSystem();
		VerbosePrint.outPrintln(System.out, clParams, 1);
		VerbosePrint.streamPrint(System.out, clParams, 1,
		"Final throughput score for the system: ");
		VerbosePrint.streamPrintf (System.out, clParams, 1, 
				"%.3f" + VerbosePrint.getLineSeparator(), 
				rtmb.results.getThroughputScoreForSystem());
		VerbosePrint.streamPrint(System.out, clParams, 1,
		"Final consistency score for the system: ");
		VerbosePrint.streamPrintf (System.out, clParams, 1, 
				"%.3f" + VerbosePrint.getLineSeparator(), 
				rtmb.results.getConsistencyScoreForSystem());
		VerbosePrint.outPrintln(System.out, clParams, 1);
		VerbosePrint.streamPrint(System.out, clParams, 1, 
		"Final Score for system: ");
		VerbosePrint.streamPrintf (System.out, clParams, 1,
				"%.3f" + VerbosePrint.getLineSeparator(),
				rtmb.results.getFinalScoreForSystem());
		/*
		 * Now put stuff in the report directory for future lookup.
		 */
		ReportGenerator rGen = new ReportGenerator(rtmb, rtmb.testParams);
		rGen.generateReportForAllTests(rtmb.results);
		System.exit(status);
	}
}
