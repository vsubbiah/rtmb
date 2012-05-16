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

import java.lang.reflect.Constructor;
import java.lang.reflect.InvocationTargetException;

import com.ibm.realtime.rtmb.params.TestInfo;
import com.ibm.realtime.rtmb.tests.util.RTSJCheck;
import com.ibm.realtime.rtmb.tests.util.SpecTest;

public class BenchMarkTestFactory {
	private static BenchMarkTestFactory instance = new BenchMarkTestFactory();

	public static BenchMarkTestFactory getInstance() {
		return instance;
	}

	public SpecTest getTest(TestInfo test) throws ClassNotFoundException {
		SpecTest specTest = null;
		String testName = test.getTestName();
		Class<?> clz = null;
		String fullClassName = "com.ibm.realtime.rtmb.tests." + testName;
		try {
			clz = Class.forName(fullClassName);
		} catch (ClassNotFoundException e) {
			if (RTSJCheck.checkIfRTSJ()) {
				fullClassName = "com.ibm.realtime.rtmb.tests.RTSJSpecificTests." + testName;
				try {
					clz = Class.forName(fullClassName);
				} catch (ClassNotFoundException e1) {
					throw e1;
				}				
			} else {
				throw e;
			}
		}
		try {
			Constructor<?> ctr = clz.getConstructor();
			specTest = (SpecTest)ctr.newInstance();
		} catch (SecurityException e) {
			e.printStackTrace();
		} catch (NoSuchMethodException e) {
			e.printStackTrace();
		} catch (IllegalArgumentException e) {
			e.printStackTrace();
		} catch (InstantiationException e) {
			e.printStackTrace();
		} catch (IllegalAccessException e) {
			e.printStackTrace();
		} catch (InvocationTargetException e) {
			e.printStackTrace();
		}
		return specTest;
	}
}
