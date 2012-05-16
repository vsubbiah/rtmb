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
 
package com.ibm.realtime.rtmb.tests.util.RTSJSpecificTestUtils;

import javax.realtime.AbsoluteTime;
import javax.realtime.Clock;

import com.ibm.realtime.rtmb.tests.util.BenchMarkClock;
import com.ibm.realtime.rtmb.util.RTSJSpecificTest;

@RTSJSpecificTest
public class RTBenchMarkClock extends BenchMarkClock {
	AbsoluteTime currentTime = null;
	private static Clock c = Clock.getRealtimeClock();
	
	public RTBenchMarkClock() {
		this.currentTime = c.getTime();
	}
	
	@Override
	public long getTime() {
		return convertToNanoseconds();
	}
	
	private long convertToNanoseconds() {
		long msecs = currentTime.getMilliseconds() ;
		long nanosecs = currentTime.getNanoseconds(); 
		return ( msecs * 1000000 + nanosecs);  
	}
	
	@Override
	public long diffTime(long time1) {
		return (convertToNanoseconds() - time1);
	}
}
