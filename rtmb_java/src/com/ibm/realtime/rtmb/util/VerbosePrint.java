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
 
package com.ibm.realtime.rtmb.util;

import java.io.PrintStream;
import java.util.Properties;

import com.ibm.realtime.rtmb.params.TestParams;

public class VerbosePrint {
	/*
	 * Do not allow any instances to be created.
	 */
	private VerbosePrint() {

	}
	public static void streamPrintln(PrintStream pStream, TestParams clParams, int verboseLevel, String whatToPrint) {
		String printLineWithNewLine = whatToPrint + getLineSeparator();
		if (clParams.isVerbose()) {
			if (clParams.getVerboseLevel() >= verboseLevel) {
				pStream.print(printLineWithNewLine);
			}
		}
	}

	public static void streamPrint (PrintStream pStream, 
			TestParams clParams, int verboseLevel, String whatToPrint) {
		if (clParams.isVerbose()) {
			if (clParams.getVerboseLevel() >= verboseLevel) {
				pStream.print(whatToPrint);
			}
		}
	}

	public static String getLineSeparator () {
		Properties p = System.getProperties();
		return p.getProperty("line.separator");
	}

	public static String getFileSeparator () {
		Properties p = System.getProperties();
		return p.getProperty("file.separator");
	}

	public static void outPrintln(PrintStream pStream, 
			TestParams clParams, int verboseLevel) {
		streamPrintln (pStream, clParams, verboseLevel, "");
	}

	public static void outPrintln (String whatToPrint) {
		System.out.println(whatToPrint);
	}
	public static void errPrintln (String whatToPrint) {
		System.err.println(whatToPrint);
	}

	public static void outPrint (String whatToPrint) {
		System.out.print(whatToPrint);
	}

	public static void errPrint (String whatToPrint) {
		System.err.print(whatToPrint);
	}

	public static void streamPrintln (PrintStream stream, String whatToPrint) {
		stream.println(whatToPrint);
	}

	public static void outPrintln () {
		System.out.println();
	}
	public static void errPrintln () {
		System.err.println();
	}

	public static void streamPrintln (PrintStream stream) {
		stream.println();
	}

	public static void streamPrintf (PrintStream out, TestParams clParams, 
			int verboseLevel, String format, double value) {
		if (clParams.isVerbose()) {
			if (clParams.getVerboseLevel() >= verboseLevel) {
				out.printf(format, value);
			}
		}
	}
	
	public static void outPrintf(PrintStream out, TestParams clParams, 
			int verboseLevel, String format, double value) {
		if (clParams.isVerbose()) {
			if (clParams.getVerboseLevel() >= verboseLevel) {
				out.printf(format, value);
			}
		}
	}
	
	public static void streamPrintf(PrintStream out, TestParams clParams, 
			int verboseLevel, String format, int value) {
		if (clParams.isVerbose()) {
			if (clParams.getVerboseLevel() >= verboseLevel) {
				out.printf(format, value);
			}
		}
	}
	
	public static void streamPrintf(PrintStream out, String format, double value) {
		out.printf(format, value);
	}
	public static void streamPrintf(PrintStream out, String format, long value) {
		out.printf(format, value);
	}
	public static void streamPrintf(PrintStream out, String format, int value) {
		out.printf(format, value);
	}
	
	public static void streamPrint (PrintStream stream, String whatToPrint) {
		stream.print(whatToPrint);
	}
	
	public static void testRetry(PrintStream out, TestParams tParams, String name) {
		VerbosePrint.streamPrintln (out, tParams, 1, getLineSeparator() + name + ": Retrying test with bigger work quantum to reduce variance... ");
	}
	
	public static void testStart(PrintStream out, TestParams tParams, String name, int testIterations) {
		VerbosePrint.outPrintln (out, tParams, 1);
		String title = "Test Results for " + name;
		VerbosePrint.streamPrintln (out, tParams, 1, title);
		StringBuilder underline = new StringBuilder();
		for (int i=0; i<title.length(); ++i) {
			underline.append("=");
		}
		VerbosePrint.streamPrintln (out, tParams, 1, underline.toString());
		VerbosePrint.outPrintln (out, tParams, 1);

		VerbosePrint.streamPrintln (out, tParams, 1, 
				name + ": Total number of iterations: " +
				testIterations);
	}
	
	public static void configStart(PrintStream out, TestParams testParams) {
		VerbosePrint.outPrintln(out, testParams, 1);
		VerbosePrint.streamPrintln(out, testParams, 1, "Running the configuration tests...");
		VerbosePrint.streamPrintln(out, testParams, 1, "==================================");
	}
	
	public static void microBenchStart(PrintStream out, TestParams testParams) {
		VerbosePrint.outPrintln(out, testParams, 1);
		VerbosePrint.streamPrintln(out, testParams, 1, "Running micro-benchmarks...");
		VerbosePrint.streamPrintln(out, testParams, 1, "===========================");
	}
	
	public static void diffTime(PrintStream out, TestParams tParams, String name, long timeDiff) {
		VerbosePrint.streamPrint (out, tParams, 2, name + ": Difference between start and end times: ");
		VerbosePrint.streamPrintf(System.out, tParams, 2, "%.3f us" + 
				VerbosePrint.getLineSeparator(), 
				VerbosePrint.roundToMicroSec(timeDiff));
	}

	public static double roundToMicroSec (double nanoSec) {
		return (nanoSec / 1000);
	}
	
	public static double roundToMicroSec (long nanoSec) {
		double d = nanoSec;
		return (d / 1000.0);
	}
	
	
}

