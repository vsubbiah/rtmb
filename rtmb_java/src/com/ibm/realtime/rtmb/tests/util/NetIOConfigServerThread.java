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

import java.io.BufferedInputStream;
import java.io.BufferedOutputStream;
import java.io.IOException;
import java.net.ServerSocket;
import java.net.Socket;

import com.ibm.realtime.rtmb.tests.impl.BenchMarkThreadFactory;

public class NetIOConfigServerThread implements Runnable {
	private ServerSocket serverSocket;
	private boolean closeConnection;

	public NetIOConfigServerThread (PriorityType priority) {
		/* Create a server socket */
		try {
			this.serverSocket = new ServerSocket(
					TestFinalDefinitions.NETIO_CONFIG_SVR_PORT);
			serverSocket.setSoTimeout(
					TestFinalDefinitions.NETIO_CONFIG_SOCKET_TIMEOUT);
		} catch (IOException e) {
			e.printStackTrace();
		}
		this.closeConnection = false;
		BenchMarkThread bmThread = BenchMarkThreadFactory.getInstance().getThread(priority, this);
		bmThread.execute();
	}

	public void setCloseConnection(boolean closeConnection) {
		this.closeConnection = closeConnection;
	}

	public void run() {
		serviceConnections();
	}

	private void serviceConnections() {
		Socket newConnSocket = null;
		BufferedInputStream bInputStream = null;
		BufferedOutputStream bOutputStream = null;

		try {
			newConnSocket = serverSocket.accept();
		} catch (IOException e) {
			e.printStackTrace();
			return;
		}
		/*
		 * Read Client request and respond.
		 */
		byte [] serverByteArray = 
			new byte[TestFinalDefinitions.NETIO_CONFIG_BSIZE];
		try {
			bInputStream = new BufferedInputStream(
					newConnSocket.getInputStream(), 
					TestFinalDefinitions.NETIO_CONFIG_BSIZE);
			bOutputStream = new BufferedOutputStream(
					newConnSocket.getOutputStream(),
					TestFinalDefinitions.NETIO_CONFIG_BSIZE);
		} catch (IOException e) {
			e.printStackTrace();
		}
		while (!this.closeConnection) {
			try {
				bInputStream.read(serverByteArray);
				bOutputStream.write(serverByteArray);
			} catch (IOException e) {
			}
		}
		try {
			serverCleanup();
		} catch (IOException e) {
		}
	}

	private void serverCleanup() throws IOException {
		serverSocket.close();
	}
}