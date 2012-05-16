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

import com.ibm.realtime.rtmb.results.TestResults;

public class TestIncompleteException extends Exception {

	public TestIncompleteException(String exception) {
		super(exception);
	}
	public TestIncompleteException(String exception, TestResults [] tResults) {
		super(exception);
		this.testResults = tResults;
	}
	private TestResults [] testResults = null;
	private static final long serialVersionUID = -674150361741598686L;
	
	public TestResults[] getTestResults() {
		return testResults;
	}
	
}
