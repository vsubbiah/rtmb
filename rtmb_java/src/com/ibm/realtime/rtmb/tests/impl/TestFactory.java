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
import com.ibm.realtime.rtmb.tests.util.SpecTest;

public class TestFactory {
	private static TestFactory instance = new TestFactory();

	public static TestFactory getInstance() {
		return instance;
	}

	public SpecTest getTest(TestInfo test) {
		SpecTest specTest = null;
		String testName = test.getTestName();
		try {
			String fullClassName = "com.ibm.realtime.rtmb.tests." + testName;
			Class<?> clz = Class.forName(fullClassName);
			Constructor<?> ctr = clz.getConstructor();
			specTest = (SpecTest)ctr.newInstance();
		} catch (ClassNotFoundException e) {
			e.printStackTrace();
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
