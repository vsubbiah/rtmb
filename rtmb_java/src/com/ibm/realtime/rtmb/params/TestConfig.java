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
 
package com.ibm.realtime.rtmb.params;

import java.io.IOException;
import java.util.Iterator;
import java.util.LinkedList;

import com.ibm.realtime.rtmb.tests.util.RTModes;
import com.ibm.realtime.rtmb.util.FilePropertyIterator;
import com.ibm.realtime.rtmb.util.VerbosePrint;

public class TestConfig {

	private String inputFilename;

	/** Collection of tests to be run */
	private LinkedList<TestInfo> tests;

	/** Expected times for soft real time systems. */
	private RealTimeValues softRealTimeValues;

	/** Expected times for hard real time systems. */
	private RealTimeValues hardRealTimeValues;

	/** Expected times for guaranteed real time systems. */
	private RealTimeValues guaranteedRealTimeValues;


	/**
	 * Constructor
	 * @param clp String containing the command line parameters.
	 * @throws IOException 
	 * @throws InvalidInputFileFormatException 
	 */
	public TestConfig(TestParams tParams) throws InvalidInputFileFormatException, IOException {
		this.inputFilename = tParams.getInputFilename();
		this.hardRealTimeValues = new RealTimeValues(RTModes.HARD_RT);
		this.softRealTimeValues = new RealTimeValues(RTModes.SOFT_RT);
		this.guaranteedRealTimeValues = new RealTimeValues(RTModes.GUARANTEED_RT);
		this.tests = new LinkedList<TestInfo>();
		if (parse(tParams.getTestNames())) {
			tParams.printSelectedOptions();	
		} else {
			throw new InvalidInputFileFormatException("Internal Error: No user specified test present in configuration file. Aborting...");
		}
	}

	/**
	 * Input file parser
	 * @return A fully initialized InputParameters
	 * @throws InvalidInputFileFormatException if the input has formatting errors.
	 * @throws IOException If the input file is not readable
	 */

	private boolean parse(LinkedList<String> listOfTestNames) throws InvalidInputFileFormatException, IOException {
		String values[] = null;
		TestInfo t = null;
		boolean newTest = false;

		if (System.getProperty("useXML") == null) {
			FilePropertyIterator it = new FilePropertyIterator(inputFilename);
			while(it.hasNext()) {
				String token=(String)it.next();
				if (token.length() != 0) {
					if (token.equals("test")) {
						if (!newTest) {
							newTest = true;
						} else {
							throw new InvalidInputFileFormatException("Invalid format " +
									"within the input file " + inputFilename);
						}
						t = new TestInfo();
					} else if (token.equals("/test")) {
						if (newTest) {
							newTest = false;
						} else {
							throw new InvalidInputFileFormatException("Invalid format " +
									"within the input file " + inputFilename);
						}
						tests.addLast(t);
					} else if (token.charAt(0) == '\n') {
						continue;
					} else if (token.charAt(0) == ' ') {
						continue;
					} else if (token.charAt(0) == '#') {
						continue;
					} else {
						values = token.split("=");
						if (values[0].equals("iterations")) {
							t.setTestIterations(Integer.parseInt(values[1]));
						} else if (values[0].equals("name")) {
							t.setTestName(values[1]);
						} else if (values[0].equals("desc")) {
							t.setTestDesc(values[1]);
						} else if(values[0].equals("config_test")) {
							if (Integer.parseInt(values[1]) == 1) {
								t.setConfigTest(true);
							} else {
								t.setConfigTest(false);
							}
						} else if(values[0].equals("reference_score")) {
							t.setReferenceScore(Double.parseDouble(values[1]));
						} else if(values[0].equals("threshold")) {
							t.setThreshold(Integer.parseInt(values[1]));
						} else if (values[0].equals("hard.acceptable_percentage")) {
							hardRealTimeValues.setAcceptablePercent(Float.parseFloat(values[1]));
						} else if (values[0].equals("hard.range_percentage")) {
							hardRealTimeValues.setRangePercent(Float.parseFloat(values[1]));
						} else if (values[0].equals("hard.multiplier")) {
							hardRealTimeValues.setMultiplier(Integer.parseInt(values[1]));
						} else if (values[0].equals("soft.acceptable_percentage")) {
							softRealTimeValues.setAcceptablePercent(Float.parseFloat(values[1]));
						} else if (values[0].equals("soft.range_percentage")) {
							softRealTimeValues.setRangePercent(Float.parseFloat(values[1]));
						} else if (values[0].equals("soft.multiplier")) {
							softRealTimeValues.setMultiplier(Integer.parseInt(values[1]));
						} else if (values[0].equals("guaranteed.acceptable_percentage")) {
							guaranteedRealTimeValues.setAcceptablePercent(Float.parseFloat(values[1]));
						} else if (values[0].equals("guaranteed.range_percentage")) {
							guaranteedRealTimeValues.setRangePercent(Float.parseFloat(values[1]));
						} else if (values[0].equals("guaranteed.multiplier")) {
							guaranteedRealTimeValues.setMultiplier(Integer.parseInt(values[1]));
						}
					}
				}
			}
		} else {
			// Handling an XML Input file needs TBD
		}
		/*
		 * Iterate through the list of tests in the input file
		 * and check if the user supplied test can be run or not.
		 */
		if (listOfTestNames != null) {
			int numberOfTests = listOfTestNames.size();
			Iterator<String> iter = listOfTestNames.iterator();
			Iterator<TestInfo> iter1 = tests.iterator();
			boolean present = false;
			while (iter.hasNext()) {
				String testName = iter.next();
				while (iter1.hasNext()) {
					TestInfo tInfo = iter1.next();
					if (tInfo.getTestName().equals(testName)) {
						present = true;
					}
				}
				if (!present) {
					VerbosePrint.errPrintln("Internal Error: Unable to locate micro-benchmark " + 
							testName + " in the configuration file. Skipping it...");
					iter.remove();
					numberOfTests--;
				}
			}
			if (numberOfTests == 0) {
				return false;
			} else {
				return true;
			}			
		}
		return true;
	}
	
	/**
	 * Returns the BenchMarkTest object of the corresponding index.
	 * @param index Index of the the BenchMarkTest in InputParams.
	 * @return BenchMarkTest object.
	 */
	public TestInfo getTestEntry(int index) {
		return tests.get(index);
	}

	/**
	 * Returns the number of tests.
	 * @return num Number of tests.
	 */
	public int getListSize() {
		return tests.size();
	}

	/**
	 * Adds a new test BenchMarkTest object entry.
	 * @param t BenchMarkTest object that should be added.
	 */
	public void addTestEntry(TestInfo t) {
		tests.add(t);
	}

	public RealTimeValues getGuaranteedRealTimeValues() {
		return guaranteedRealTimeValues;
	}

	public RealTimeValues getHardRealTimeValues() {
		return hardRealTimeValues;
	}

	public RealTimeValues getSoftRealTimeValues() {
		return softRealTimeValues;
	}

	/**
	 * @return the tests
	 */
	public LinkedList<TestInfo> getTests() {
		return tests;
	}

	/**
	 * @param tests the tests to set
	 */
	public void setTests(LinkedList<TestInfo> tests) {
		this.tests = tests;
	}
}
