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

import com.ibm.realtime.rtmb.tests.util.BenchMarkClock;
import com.ibm.realtime.rtmb.tests.util.RTSJCheck;
import com.ibm.realtime.rtmb.tests.util.StdBenchMarkClock;
import com.ibm.realtime.rtmb.tests.util.RTSJSpecificTestUtils.RTBenchMarkClock;

public class BenchMarkClockFactory {
	private static BenchMarkClockFactory instance = new BenchMarkClockFactory();
	
	public static BenchMarkClockFactory getInstance() {
		return instance;
	}
	
	public BenchMarkClock getClock() {
		if( RTSJCheck.checkIfRTSJ() ) {
			RTBenchMarkClock rtBenchMarkClock = new RTBenchMarkClock();
			return rtBenchMarkClock;
		} else {
			StdBenchMarkClock stdBenchMarkClock = new StdBenchMarkClock();
			return stdBenchMarkClock;
		}
	}
}


