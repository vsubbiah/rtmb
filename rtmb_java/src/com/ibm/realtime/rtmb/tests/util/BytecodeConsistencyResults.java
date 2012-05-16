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
import com.ibm.realtime.rtmb.util.NoPreCompileAnnotation;

@NoPreCompileAnnotation
public class BytecodeConsistencyResults {
	private TestResults results [] = null;
	private int currentResult = 0;
	
	public BytecodeConsistencyResults(int totalTestResults, int iterations) {
		this.results = new TestResults[totalTestResults];
		for (int i = 0; i < totalTestResults; i++) {
			results[i] = new TestResults(TestId.BYTECODE_CONSISTENCY_TEST, iterations, true);
		}
	}
	
	public TestResults getNextTestResult() {
		return results[currentResult++];
	}

	public TestResults[] getResults() {
		return results;
	}
}