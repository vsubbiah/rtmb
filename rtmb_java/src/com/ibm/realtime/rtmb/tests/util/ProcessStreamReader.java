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

import java.io.BufferedReader;
import java.io.IOException;
import java.io.InputStream;
import java.io.InputStreamReader;
import java.io.PrintStream;

public class ProcessStreamReader extends Thread {
	InputStream is;
	PrintStream out;
	PrintStream err;
	boolean _serverStarted;
	ProcessStreamReader(InputStream is, PrintStream out, PrintStream err) {
		this.is = is;
		this.out = out;
		this.err = err;
	}
	public void run() {
		try {
			String line;
			BufferedReader rdr = new BufferedReader(new InputStreamReader(is));
			while ((line = rdr.readLine()) != null) {
				out.println(line);
			}
		} catch (IOException ioe) {
			err.println("Error reading child process output");
		}
		out.flush();
	}
}