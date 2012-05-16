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

import java.io.IOException;

import com.ibm.realtime.rtmb.tests.MultiCpuTest;
import com.ibm.realtime.rtmb.tests.impl.BenchMarkThreadFactory;

/**
 * This thread class contains the code to run multicpu tests. This class will
 * create High ,medium and low priority threads simultaneously . Based on the
 * time taken for completion of tasks determines whether the threads have
 * completed according to priority.
 */

public class MultiCpuHelperThread implements Runnable {
	private BenchMarkThread thrd = null;
	private MultiCpuTest multiCpu = null;
	private int subIterations;
	private int iterations;
	private long timeToStart;
	private long sleepTime;
	private String name;
	private PriorityType priority;
	private boolean runPassed = true;
	private int matrixSize = 1;
	private String jvmOptions = null;

	public MultiCpuHelperThread(int subIterations, int iterations,
			long sleepTime, long timeToStart, String name,
			PriorityType priority, MultiCpuTest multiCpu, String jvmOptions,
			int matrixSize) {
		this.subIterations = subIterations;
		this.iterations = iterations;
		this.timeToStart = timeToStart;
		this.sleepTime = sleepTime;
		this.name = name;
		this.priority = priority;
		this.multiCpu = multiCpu;
		this.runPassed = true;
		this.matrixSize = matrixSize;
		thrd = BenchMarkThreadFactory.getInstance().getThread(
				PriorityType.HighPriority, this);
		this.jvmOptions = jvmOptions;
	}

	public void start() {
		try {
			thrd.execute();
		} catch (Exception e) {
			e.printStackTrace();
		}
	}

	public String getFileName() {
		return name;
	}

	public void run() {
		String priorityString = null;

		if (priority == PriorityType.HighPriority) {
			priorityString = "1";
		} else {
			priorityString = "0";
		}

		String commandExec = System.getProperty("java.home") + "/bin/java";
		String commandLine = null;
		commandLine = commandExec + " " + this.jvmOptions + " -classpath " 
				+ System.getProperty("java.class.path") 
				+ " com/ibm/realtime/rtmb/tests/util/MultiCpuImpl "
				+ iterations + " " + subIterations + " " + timeToStart + " "
				+ sleepTime + " " + name + " " + priorityString + " "
				+ matrixSize;				
		multiCpu.incrementCounter();
		Process p = null;

		try {
			p = Runtime.getRuntime().exec(commandLine);
			ProcessStreamReader psrOut = new ProcessStreamReader(p.getInputStream(), System.out, System.err);
			ProcessStreamReader psrErr = new ProcessStreamReader(p.getErrorStream(), System.out, System.err);
			psrOut.start();
			psrErr.start();
			p.waitFor();
			int rc = p.exitValue();
			if (rc != 0) {
				System.err.println ("Unexpected exit value from sub-process. Exit value = " + p.exitValue());
			}
		} catch (IOException e) {
			e.printStackTrace();
		} catch (InterruptedException e) {
			e.printStackTrace();
		}
		multiCpu.decrementCounter();

		if (multiCpu.getCounter() == 0) {
			synchronized (multiCpu) {
				multiCpu.notify();
			}
		}
	}

	public void setRunPassed(boolean runPassed) {
		this.runPassed = runPassed;
	}

	public boolean getRunPassed() {
		return runPassed;
	}
}
