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

import java.lang.reflect.Constructor;
import java.lang.reflect.Method;

import com.ibm.realtime.rtmb.util.VerbosePrint;

public class MultiCpuImpl {
	public static void main(String s[]) {
		int iterations = Integer.parseInt(s[0]);
		int subIterations = Integer.parseInt(s[1]);
		long timeToStart = Long.parseLong(s[2]);
		long sleepTime = Long.parseLong(s[3]);
		String name = s[4];
		int priority = Integer.parseInt(s[5]);
		int matrixSize = Integer.parseInt(s[6]);

		String classToCompile[] = { 
                "java.lang.Integer",
				"com.ibm.realtime.rtmb.tests.util.MatrixOperationsWithObjects",
				"com.ibm.realtime.rtmb.tests.util.MultiCpuTaskRunner"};

		try {
			Class<?> clazz = null;

			for (int i = 0; i < classToCompile.length; i++) {
				clazz = Class.forName(classToCompile[i]);
				boolean isCompiled = Compiler.compileClass(clazz);
				if (!isCompiled) {
					VerbosePrint.errPrintln("MultiCpuTest: unable to compile class "
							+ classToCompile[i]);
				}
			}

			Constructor<?> ctr = clazz.getConstructor(new Class[] { int.class,
					int.class, long.class, long.class, String.class, int.class,
					int.class });

			Object obj = ctr.newInstance(Integer.valueOf(iterations), 
					Integer.valueOf(subIterations), Long.valueOf(timeToStart), 
					Long.valueOf(sleepTime), name, Integer.valueOf(priority), 
					Integer.valueOf(matrixSize));

			Method methodId = clazz.getMethod("doWork");
			methodId.invoke(obj);
		} catch (Exception e) {
			e.printStackTrace();
		}

		System.exit(0);
	}
}
