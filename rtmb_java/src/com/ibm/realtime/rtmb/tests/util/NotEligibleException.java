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

public class NotEligibleException extends Exception {

	public NotEligibleException() {
		super();
	}
	public NotEligibleException(String msg) {
		super (msg);
	}
	/**
	 * 
	 */
	private static final long serialVersionUID = 8383485688717068254L;

}