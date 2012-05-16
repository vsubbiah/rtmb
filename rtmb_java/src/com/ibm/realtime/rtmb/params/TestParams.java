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

import java.io.PrintStream;
import java.util.LinkedList;

import com.ibm.realtime.rtmb.RTMB;
import com.ibm.realtime.rtmb.tests.ClockConfig;
import com.ibm.realtime.rtmb.tests.TimerConfig;
import com.ibm.realtime.rtmb.util.VerbosePrint;

public class TestParams {
	
	/** Set to true if the parameters were processed cleanly */
	private boolean ok; 
	
	/** Set this for verbose output */

	private boolean isVerbose;

	/** Level of verbosity */

	private int verboseLevel;

	/** Path to the file containing the input parameters. */

	private String inputFilePathName;

	/** Path to the report directory. */

	private String reportDirName;

	private int rtMode;

	/** Setting this would mean running the tests for a single iteration */

	private boolean quickRun;

	/** List of tests to be run */

	private LinkedList<String> testNames;

	/** List of JVM options that are required to launch any new java processes */
	private String jvmOptions;

	public TestParams(String[] args) {
		setDefaults();
		parse(args);
	}

	private void setDefaults() {
		this.isVerbose = false;
		this.quickRun = false;
		this.testNames = null;
		this.rtMode = 0;
		this.verboseLevel = 0;
		this.reportDirName = System.getProperty("user.dir");
		this.inputFilePathName = this.reportDirName + "/config/input.txt";
		this.jvmOptions = null;
	}

	private void parse(String[] cmdLine) {
		int i;
		boolean vmOptionsSet = false;
		ok = true;
		
		for (i = 0; i < cmdLine.length; i++) {
			if (cmdLine[i].equals("--help") || 
					cmdLine[i].equals("-h")  || 
					cmdLine[i].equals("-?") )  {
				usage(0);
			} else if (cmdLine[i].equals("--test?") ||
					cmdLine[i].equals("-t?")) {
				printTestList();
				return;
			} else if (cmdLine[i].equals("--soft-rt")) {
				rtMode = 0;
			} else if (cmdLine[i].equals("--hard-rt")){
				rtMode  = 1; 
			} else if (cmdLine[i].equals("--guaranteed-rt")) {
				rtMode = 2;
			} else if (cmdLine[i].equals("--verbose:simple") || cmdLine[i].equals("-v:simple")) {
				isVerbose = true;
				verboseLevel = 1;
			} else if (cmdLine[i].equals("--verbose:detailed") || cmdLine[i].equals("-v:detailed")) {
				isVerbose = true;
				verboseLevel = 2;
			} else if (cmdLine[i].equals("--jvm-options")){
				this.jvmOptions = cmdLine[i+1];
				vmOptionsSet = true;
				i++;
			} else if (cmdLine[i].equals("--silent") || cmdLine[i].equals("-s")) {
				isVerbose = false;
			} else if ( cmdLine[i].equals("--file") || cmdLine[i].equals("-f")) {
				if ( i+1 < cmdLine.length) {
					this.inputFilePathName = cmdLine[i+1];
				} else {
					usage(-1);
					return;
				}
				i++;
			} else if (cmdLine[i].equals("--report") || cmdLine[i].equals("-r")) {
				if (i+1 < cmdLine.length) {
					this.reportDirName = cmdLine[i+1];
				} else {
					usage(-1);
					return;
				}
				i++;
			} else if (cmdLine[i].equals("--quick") || cmdLine[i].equals("-q")) {
				this.quickRun = true;
			} else if (cmdLine[i].equals("--compliant") || cmdLine[i].equals("-c")) {
				this.quickRun = false;
			} else if (cmdLine[i].equals("--clock-accuracy") || cmdLine[i].equals("-ca")) {
				if (i+1 < cmdLine.length) {
					String time = cmdLine[i+1];
					if (ClockConfig.setClockAccuracy(time)) {
						usage(-1);
						return;
					}
				} else {
					usage(-1);
					return;
				}
				i++;
			} else if (cmdLine[i].equals("--timer-accuracy") || cmdLine[i].equals("-ta")) {
				if (i+1 < cmdLine.length) {
					String time = cmdLine[i+1];
					if (TimerConfig.setTimerAccuracy(time)) {
						usage(-1);
						return;
					}
				} else {
					usage(-1);
					return;
				}
				i++;
			} else if (cmdLine[i].equals("--test") || cmdLine[i].equals("-t")) {
				if (i+1 < cmdLine.length) {
					String testName = cmdLine[i+1];
					if (testName.equals("?")) {
						printTestList();
						return;
					} else {
						setTestNames(cmdLine[i+1]);
					}
				} else {
					usage(-1);
					return;
				}
				i++;
			} else {
				VerbosePrint.errPrintln("Invalid option \"" + cmdLine[i] + "\" specified on " +
				"the command line. Specify --help for valid options");
				usage(-1);
				return;
			}
		}
		if (!vmOptionsSet) {
			VerbosePrint.errPrintln("Error: Specify mandatory options for child " +
					"Java processes using --jvm-options switch.");
			usage(-1);
			return;
		}
	}

	/**
	 * Returns the status of isVerbose flag
	 */
	public boolean isVerbose() {
		return isVerbose;
	}

	/**
	 * Returns whether it is for Hard Real Time or Soft Real Time
	 */
	public int getRTmode() {
		return this.rtMode;
	}

	/**
	 * Return the level of verbosity
	 */
	public int getVerboseLevel() {
		return verboseLevel;
	}

	/**
	 * Returns the status of quickRun flag
	 */
	public boolean isQuickRun() {
		return quickRun;
	}

	/**
	 * Returns the filename field of this class
	 * @return Input filename
	 */
	public String getInputFilename() {
		return inputFilePathName;
	}

	/**
	 * Returns the report directory path name
	 * @return Report directory name
	 */
	public String getReportDirName() {
		return reportDirName;
	}

	/**
	 * Returns the test names field of this class
	 * @return Arraylist of Strings containing test names
	 */
	public LinkedList<String> getTestNames() {
		return testNames;
	}

	/**
	 * Sets the test names field to the user supplied list of tests 
	 * @param test A ':' separated list of test names supllied with the '--test' option.
	 */
	private void setTestNames(String test) {
		testNames = new LinkedList<String>();
		String tests[] = test.split(":");
		for (int x = 0; x < tests.length; x++) {
			testNames.add(tests[x]);
		}
	}

	private void usage(int exitCode) {
		VerbosePrint.outPrintln ("Usage: java -jar " + RTMB.benchmarkName() + ".jar {<options>}");
		VerbosePrint.outPrintln ("Where options should be:");
		VerbosePrint.outPrintln ( "[--help|-?|-h]\t\t\tPrint the " +
		"command-line syntax.");
		VerbosePrint.outPrintln( "[--silent|-s]\t\t\tSilent run " +
		"(default).");
		VerbosePrint.outPrintln( "[--verbose|-v]:<verboseLevel>\tVerbose " +
		"output. Opposite of -s.");
		VerbosePrint.outPrintln("\t\t\t\tWhere <verboseLevel> should be one of:");
		VerbosePrint.outPrintln("\t\t\t\t   simple = simple verbose output");
		VerbosePrint.outPrintln("\t\t\t\t   detailed = detailed verbose output");
		VerbosePrint.outPrintln( "[--file|-f] <file>\t\tPath " +
		"for input file. Ex:\"-f ./config/input.txt\".");
		VerbosePrint.outPrintln( "[--report|-r] <dir>\t\tPath " +
		"to report directory. Defaults to $PWD.");
		VerbosePrint.outPrintln( "[--quick|-q]\t\t\tPerform a " +
		"quick single iteration run.");
		VerbosePrint.outPrintln( "[--compliant|-c]\t\tPerform " +
		"compliant run. Opposite of -q (default).");
		VerbosePrint.outPrintln( "[--soft-rt]\t\t\tPerform the test in the soft " +
		"real-time mode (Default).");
		VerbosePrint.outPrintln( "[--hard-rt]\t\t\tPerform the test in the hard " +
		"real-time mode.");
		VerbosePrint.outPrintln( "[--guaranteed-rt]\t\tPerform the test in the guaranteed " +
		"real-time mode.");
		VerbosePrint.outPrintln( "[--clock-accuracy|-ca <accuracy>]\t\tProvide the clock accuracy to use, skipping the clock configuration " +
				"(forces non-compliance)" );
		VerbosePrint.outPrintln("\t\t\t\tWhere <accuracy> should be expressed in ns, us, or ms:");
		VerbosePrint.outPrintln( "[--timer-accuracy|-ta <accuracy>]\t\tProvide the timer accuracy to use, skipping the timer configuration " +
		"(forces non-compliance)" );
VerbosePrint.outPrintln("\t\t\t\tWhere <accuracy> should be expressed in ns, us, or ms:");
		VerbosePrint.outPrintln( "[--jvm-options]\t\t\tOptions to be passed to any" +
				" new JVMs launched from the micro-benchmark suite.");
		VerbosePrint.outPrintln( "[--test|-t] [<tests>]\t\t" +
		"Tests to run separated by colon. Default = all.");
		VerbosePrint.outPrintln( "\t\t\t\tType [--test|-t]? for" +
		" list of supported tests.");
		
		ok = false;
	}

	private void printTestList() {
		VerbosePrint.outPrintln ("Usage: java -jar " + RTMB.benchmarkName() + ".jar [-t|--test] [<tests>]*");
		VerbosePrint.outPrintln ("Where <tests> could be one or more of:");
		VerbosePrint.outPrintln ("IntegralRateTest\tMeasures the integral rate of " +
		"the underlying system");
		VerbosePrint.outPrintln ("FloatRateTest\t\tMeasures the floating point rate" +
		" of the underlying system");
		VerbosePrint.outPrintln ("MemCacheTest\t\tVerifies that random page accesses" +
		" and possible page cache misses will not cause indeterminism");
		VerbosePrint.outPrintln ("DiskIOReadTest\t\tVerifies that disk read requests" +
		" are deterministic");
		VerbosePrint.outPrintln ("DiskIOWriteTest\t\tVerifies that disk write requests" +
		" are deterministic");
		VerbosePrint.outPrintln ("NetIOTest\t\tVerifies that read and write requests" +
		" using TCP/IP sockets are deterministic");
		VerbosePrint.outPrintln ("TimeAccuracyTest\t\tVerifies that the " +
		"clock and timer work consistently.");
		VerbosePrint.outPrintln ("EventDispatchTest\t\tVerifies that events are " +
		"dispatched with consistent latencies.");
		VerbosePrint.outPrintln ("ThreadPriorityTest\t\tVerifies that higher " +
		"priority threads preempt lower priority threads.");
		VerbosePrint.outPrintln ("LockConsistencyTest\t\tVerifies that " +
		"uncontended locks are acquired with consistent overhead.");
		VerbosePrint.outPrintln ("PeriodicEvents\t\tVerifies that " +
		"periodic real-time events are dispatched consistently (RTSJ specific test).");
		VerbosePrint.outPrintln ("GarbageGenerationTest\tVerifies that garbage generation " +
				"(high object allocation rates) does not cause unexpected delays or inconsistent " +
		"application performance.");
		VerbosePrint.outPrintln ("CompilationTest\t\tVerifies that dynamic compilation does " +
		"not cause unexpected delays or result in application performance to degrade.");
		VerbosePrint.outPrintln ("BytecodeConsistencyTest\tVerifies that all byte-codes " +
		"run with consistent performance.");
		VerbosePrint.outPrintln ("MultiThreadTest\t\tVerifies that systems with multiple " +
		"CPUs can scale reasonably across multiple application threads in a single process.");
		VerbosePrint.outPrintln ("MultiCPU\t\tVerifies that systems with multiple CPUs " +
		"can scale reasonably across multiple runtime environments without loss of determinism.");
		VerbosePrint.outPrintln ("NHRTSupportTest\t\tVerifies that NHRTs are not " +
				"pre-empted by lower priority threads including the system garbage collector " +
		"(RTSJ specific test).");
		VerbosePrint.outPrintln ("ScopedMemoryTest\tVerifies that access to scoped and " +
		"immortal memory is consistent (RTSJ specific test).");
		
		ok = false;
	}

	private String vLevelInString() {
		if (verboseLevel == 1) {
			return "Simple";
		} else {
			return "Detailed";
		}
	}

	void printSelectedOptions() {
		PrintStream out = System.out;
		VerbosePrint.streamPrintln (out, this, 1, "Summary of test options selected:");
		VerbosePrint.streamPrintln (out, this, 1, "=================================");
		VerbosePrint.streamPrintln (out, this, 1, "");
		VerbosePrint.streamPrintln (out, this, 1, "Input Parameter file: " + getInputFilename());
		VerbosePrint.streamPrintln (out, this, 1, "Report Directory: " + getReportDirName());
		VerbosePrint.streamPrintln (out, this, 1, "Verbose mode enabled.");
		VerbosePrint.streamPrintln (out, this, 1, "Verbose Level is set to: " + vLevelInString());
		if (isQuickRun()) {
			VerbosePrint.streamPrintln (out, this, 1, "Performing a quick single iteration run.");
		} else {
			VerbosePrint.streamPrintln (out, this, 1, "Performing a compliant run.");
		}
		if (this.getTestNames() != null) {
			VerbosePrint.streamPrintln (out, this, 1, "Tests requested to be run:");
			LinkedList<String> testNames = getTestNames();
			for (int i = 0; i < testNames.size(); i++) {
				VerbosePrint.streamPrintln (out, this, 1,
						i+1 + ".\t" + testNames.get(i));
			}
		} else {
			VerbosePrint.streamPrintln (out, this, 1, "No user specified tests. Running all the tests.");
		}
		if (rtMode == 0) {
			VerbosePrint.streamPrintln (out, this, 1, "Soft real-time mode option selected.");
		} else if (rtMode == 1) {
			VerbosePrint.streamPrintln (out, this, 1, "Hard real-time mode option selected.");
		} else {
			VerbosePrint.streamPrintln (out, this, 1, "Guaranteed real-time mode option selected.");
		}
	}

	public void setVerbose(boolean isVerbose) {
		this.isVerbose = isVerbose;
	}

	public String getJvmOptions() {
		return jvmOptions;
	}

	public boolean ok() {
		return ok;
	}
}
