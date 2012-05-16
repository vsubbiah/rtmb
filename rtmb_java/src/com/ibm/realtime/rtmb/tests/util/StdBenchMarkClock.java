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

public class StdBenchMarkClock extends BenchMarkClock {
	private Long currentTime = null;
		
	public StdBenchMarkClock() {
		this.currentTime = Long.valueOf(System.nanoTime());
	}
	
	@Override
	public long getTime() {
		return currentTime;
	}
	
	private long convertToNanoseconds() {
		return currentTime.longValue();   
	}
	
	@Override
	public long diffTime(long time1) {
		return (convertToNanoseconds() - time1);
	}
}
