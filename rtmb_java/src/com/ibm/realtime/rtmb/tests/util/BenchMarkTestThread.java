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

import com.ibm.realtime.rtmb.params.RealTimeValues;
import com.ibm.realtime.rtmb.params.TestInfo;
import com.ibm.realtime.rtmb.params.TestParams;
import com.ibm.realtime.rtmb.results.TestResults;

public abstract class BenchMarkTestThread {
	private SpecTest testToRun;
	private TestResults[] results;
	private TestInfo tInfo;
	private TestParams tParams;
	private RealTimeValues rtValues;
	private boolean testNotApplicable;
	private boolean testFailed;
	private String testFailedMessage;
	
	protected BenchMarkTestThread(PriorityType priority, SpecTest testToRun, 
			TestInfo ti, RealTimeValues rtv, TestParams tp) {
		this.tInfo = ti;
		this.tParams = tp;
		this.rtValues = rtv;
		this.testToRun = testToRun;
		this.results = null;
		this.testNotApplicable = false;
		this.testFailed = false;
		this.testFailedMessage = null;
	}
	public abstract void execute();
	public abstract void waitForThreadExit() throws InterruptedException;
	public void run() {
		try {
			this.results = testToRun.benchMarkTest(tInfo, rtValues, tParams);
		} catch (NotEligibleException e) {
			this.testNotApplicable = true;
			return;
		} catch (TestIncompleteException e) {
			this.results = e.getTestResults();
			this.testFailedMessage = e.getMessage();
		}
	}

	public TestResults[] getTestResults() throws NotEligibleException, TestIncompleteException {
		if (testNotApplicable) {
			throw new NotEligibleException("Test is not applicable to this system.");
		}
		if (testFailed) {
			throw new TestIncompleteException (testFailedMessage, results);
		}
		return results;
	}
}
