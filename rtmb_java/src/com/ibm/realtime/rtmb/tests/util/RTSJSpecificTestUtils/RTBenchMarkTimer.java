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

import javax.realtime.Clock;
import javax.realtime.RealtimeThread;
import javax.realtime.RelativeTime;

import com.ibm.realtime.rtmb.tests.util.BenchMarkTimer;
import com.ibm.realtime.rtmb.util.RTSJSpecificTest;

@RTSJSpecificTest
public class RTBenchMarkTimer extends BenchMarkTimer {
	Clock c = Clock.getRealtimeClock();
	
	@Override
	public void sleep(long milliseconds) throws InterruptedException {
		RelativeTime r = new RelativeTime(milliseconds ,0);
		RealtimeThread.sleep(c, r);
	}

	@Override
	public void sleep (long milliseconds,int nanoseconds) throws InterruptedException {
		RealtimeThread.sleep(c , new RelativeTime(milliseconds, nanoseconds));
	}

}

