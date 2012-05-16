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
 
package com.ibm.realtime.rtmb.params;

import com.ibm.realtime.rtmb.tests.util.RTModes;

public class RealTimeValues {
	private double rangePercent;
	private double acceptablePercent;
	private int multiplier;
	private RTModes rtMode;

	public RealTimeValues(RTModes rtMode) {
		this.rtMode= rtMode;
	}

	public void setMultiplier(int multiplier) {
		this.multiplier = multiplier;
	}

	public int getMultiplier() {
		return this.multiplier;
	}

	public double getAcceptablePercent() {
		return acceptablePercent;
	}

	public void setAcceptablePercent(double acceptablePercent) {
		this.acceptablePercent = acceptablePercent;
	}

	public double getRangePercent() {
		return rangePercent;
	}

	public void setRangePercent(double rangePercent) {
		this.rangePercent = rangePercent;
	}

	public RTModes getRtMode() {
		return rtMode;
	}

	public void setRtMode(RTModes rtMode) {
		this.rtMode = rtMode;
	}
}

