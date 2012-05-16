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

import java.io.File;
import java.io.FileNotFoundException;
import java.io.FileOutputStream;
import java.io.IOException;

import com.ibm.realtime.rtmb.RTMB;

public class DiskIOWriteUtils {
	private FileOutputStream fOutputStream;
	private int writeBufSize;
	private File tmpFile;

	/**
	 * Constructor
	 * @param rbsize Read buffer size.
	 */
	public DiskIOWriteUtils(int rbsize) {
		this.writeBufSize = rbsize;
		try {
			this.tmpFile = File.createTempFile(RTMB.benchmarkName(), null);
		} catch (IOException e) {
			e.printStackTrace();
		}
		try {
			this.fOutputStream = new FileOutputStream(tmpFile);
		} catch (FileNotFoundException e) {
			e.printStackTrace();
		}
	}

	/**
	 * Performs the write operation on the temp file 'loops'
	 * number of times.
	 * @param loops Number of iterations of write to be done.
	 */
	public void doWrite(int loops) {
		byte [] byteArray = new byte[writeBufSize];
		for (int i = 0; i < writeBufSize; i++) {
			byteArray[i] = 'x';
		}
		for (int i = 0; i < loops; i++) {
			try {
				fOutputStream.write(byteArray);
			} catch (IOException e) {
				e.printStackTrace();
			}
		}
	}
	public void cleanup () {
		try {
			fOutputStream.close();
			tmpFile.delete();
		} catch (IOException e) {
			e.printStackTrace();
		}
	}
}
