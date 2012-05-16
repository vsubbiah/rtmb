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
import java.io.FileInputStream;
import java.io.FileNotFoundException;
import java.io.FileOutputStream;
import java.io.IOException;

import com.ibm.realtime.rtmb.RTMB;

public class DiskIOReadUtils {
	private FileInputStream fInputStream;
	private int readBufSize;
	private File tmpFile;

	/**
	 * Constructor
	 * @param rbsize Read buffer size
	 */
	public DiskIOReadUtils(int rbsize) {
		this.readBufSize = rbsize;
		try {
			this.tmpFile = File.createTempFile(RTMB.benchmarkName(), null);
		} catch (IOException e) {
			e.printStackTrace();
		}
		try {
			this.fInputStream = new FileInputStream(tmpFile);
		} catch (FileNotFoundException e) {
			e.printStackTrace();
		}
		setupFileForRead();
	}

	private void setupFileForRead() {
		/*
		 * Fill the file with ascii text to read later
		 * during the test.
		 */
		byte [] wrBuffer = new byte [readBufSize];
		FileOutputStream fOutputStream = null;
		for (int i = 0; i < readBufSize; i++) {
			wrBuffer[i] = 'x';
		}
		try {
			fOutputStream = new FileOutputStream(tmpFile);
			fOutputStream.write(wrBuffer);
			fOutputStream.close();
		} catch (FileNotFoundException e) {
			e.printStackTrace();
		} catch (IOException e) {
			e.printStackTrace();
		}
	}

	public void doRead(int loops) {
		byte [] byteArray = new byte[readBufSize];
		for (int i = 0; i < loops; i++) {
			try {
				fInputStream.read(byteArray);
			} catch (IOException e) {
				e.printStackTrace();
			}
		}
	}

	public void cleanup () {
		try {
			fInputStream.close();
			tmpFile.delete();
		} catch (IOException e) {
			e.printStackTrace();
		}
	}
}
