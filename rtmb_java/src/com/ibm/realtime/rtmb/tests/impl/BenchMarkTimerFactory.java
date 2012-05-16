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
 
package com.ibm.realtime.rtmb.tests.impl;

import com.ibm.realtime.rtmb.tests.util.BenchMarkTimer;
import com.ibm.realtime.rtmb.tests.util.RTSJCheck;
import com.ibm.realtime.rtmb.tests.util.StdBenchMarkTimer;
import com.ibm.realtime.rtmb.tests.util.RTSJSpecificTestUtils.RTBenchMarkTimer;

public class BenchMarkTimerFactory {
	private static BenchMarkTimerFactory instance = new BenchMarkTimerFactory();
	
	public static BenchMarkTimerFactory getInstance() {
		return instance;
	}

	public BenchMarkTimer getTimer() {
		if(RTSJCheck.checkIfRTSJ()) {
			RTBenchMarkTimer rtBenchMarkTimer = new RTBenchMarkTimer();
			return rtBenchMarkTimer;
		} else {
			StdBenchMarkTimer stdBenchMarkTimer = new StdBenchMarkTimer();
			return stdBenchMarkTimer;
		}
	}
}


