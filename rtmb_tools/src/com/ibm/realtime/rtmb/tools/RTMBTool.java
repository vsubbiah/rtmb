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

package com.ibm.realtime.rtmb.tools;

import java.io.BufferedReader;
import java.io.IOException;
import java.io.InputStream;
import java.io.InputStreamReader;
import java.util.ArrayList;

class RTMBTool {
	private BufferedReader _input;
	private final static String _header = "Measured execution times:";
	private final static int _maxInputErrMsg = 20;

	RTMBTool(InputStream in) {
		_input = new BufferedReader(new InputStreamReader(in));
	}
	
	protected Double[] read() throws IOException {
		String line;
		int lineCount = 0;
		int inputErrMsg = 0;
		boolean headerFound = false;
		ArrayList<Double> list = new ArrayList<Double>();
		while ((line = _input.readLine()) != null) {
			++lineCount;
			if (line.trim().equals(_header)) {
				headerFound = true;
				break;
			}
		}
		
		if (!headerFound) {
			System.err.println("No header line found before data.");
			System.err.println("Expected header line: <" + _header + ">");
		}
		
		while ((line = _input.readLine()) != null) {
			++lineCount;
			line = line.trim();
			int index = line.indexOf(":");
			if (index >= 0) {
				String number = line.substring(index+1).trim();
				index = number.indexOf(" ");
				if (index >= 0) {
					number = number.substring(0,index).trim();
				}
				try {
					Double val = Double.parseDouble(number);
					list.add(val);
				} catch (NumberFormatException nfe) {
					if (inputErrMsg < _maxInputErrMsg) {
						System.err.println(number);
						System.err.println("Line: " + lineCount + " did not contain a valid number. Line skipped");
						++inputErrMsg;
					} else if (inputErrMsg == _maxInputErrMsg) {
						System.err.println("...maximum error messages printed");
						++inputErrMsg;
					}
				}
			}
		}
		Double[] dummy = new Double[0];
		return list.toArray(dummy);
	}

}
