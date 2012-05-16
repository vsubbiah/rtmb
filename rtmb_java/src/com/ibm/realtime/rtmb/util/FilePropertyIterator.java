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
 
package com.ibm.realtime.rtmb.util;

import java.io.BufferedReader;
import java.io.FileReader;
import java.io.IOException;
import java.util.Iterator;

public class FilePropertyIterator implements Iterator<Object> {
	/** The pathname to the file to open and read */
	private String fileName;

	/** The line read from the file */
	private String currentToken;

	/** BufferedReader object */
	private BufferedReader reader;

	/**
	 * Constructor for the FilePropertyIterator class.
	 * @param filename Path to the input file. 
	 */
	public FilePropertyIterator(String filename) throws IOException {
		this.fileName = filename;
		initialize();
	}

	/**
	 * Reads a line from the input file and sets the currentToken variable 
	 * to the same.
	 * @throws IOException When read of the input file failed.
	 */
	private void initialize() throws IOException {
		reader = new BufferedReader(new FileReader(fileName));
		currentToken = reader.readLine();
	}

	/**
	 * Returns true if there is a next line in the input file to be read.
	 */
	public boolean hasNext() {
		return currentToken!=null;
	}

	/**
	 * Returns the String containing the next line of the input file.
	 * @return ret The String containing the next line of the input file.
	 */
	public Object next() {
		String ret=currentToken;
		try {
			currentToken = reader.readLine();
		} catch (IOException e) {
			e.printStackTrace();
		}
		return ret;
	}

	/**
	 * Unused for now.
	 */
	public void remove() {
		// TODO Auto-generated method stub
	}
}
