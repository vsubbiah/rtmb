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

public abstract class BytecodeConsistencySubtest implements BytecodeConsistencyInterface{
	protected int subIterations;
		
	protected BytecodeConsistencySubtest() {
		subIterations = 0;
	}
	
	protected BytecodeConsistencySubtest(int subI) {
		subIterations = subI;
	}
	
	public void setSubIterations(int subI) {
		subIterations = subI;
	}
	public int getSubIterations() {
		return subIterations;
	}

	public abstract void bcTest();
}
