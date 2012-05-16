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
 
package com.ibm.realtime.rtmb.tests;

import java.io.DataInputStream;
import java.io.DataOutputStream;
import java.io.IOException;
import java.io.InputStream;
import java.io.OutputStream;
import java.net.Socket;
import java.net.UnknownHostException;

import com.ibm.realtime.rtmb.params.RealTimeValues;
import com.ibm.realtime.rtmb.params.TestInfo;
import com.ibm.realtime.rtmb.params.TestParams;
import com.ibm.realtime.rtmb.results.TestResults;
import com.ibm.realtime.rtmb.tests.util.NetIOConfigServerThread;
import com.ibm.realtime.rtmb.tests.util.PriorityType;
import com.ibm.realtime.rtmb.tests.util.SpecTest;
import com.ibm.realtime.rtmb.tests.util.TestFinalDefinitions;
import com.ibm.realtime.rtmb.tests.util.TestId;
import com.ibm.realtime.rtmb.util.VerbosePrint;


public class NetIOTest extends SpecTest {
	Socket clientSocket;

	/**
	 * Creates and populates a request that will be used to send to
	 * server.
	 * @return request - Request in a byte array for sending to server
	 */
	private byte [] createClientRequest() {
		byte [] request = new byte [TestFinalDefinitions.NETIO_CONFIG_BSIZE];
		/* fill the byte array */
		for (int i = 0; i < TestFinalDefinitions.NETIO_CONFIG_BSIZE; i++) {
			request[i] = 'x';
		}
		return request;
	}

	@Override
	public TestResults[] benchMarkTest(TestInfo tInfo, RealTimeValues rtValues, TestParams tParams) {
		/* Other variables local to this function */
		int count = 0;
		int new_iters;
		int loops = 1;
		boolean retryMsg = false;
		OutputStream outputStream = null;
		DataOutputStream dOutputStream = null; 
		InputStream inputStream = null;
		DataInputStream dInputStream = null; 
		long startTime, endTime;
		double clockAccuracy = ClockConfig.getClockAccuracy();

		new_iters = updateIterations(tInfo, rtValues, tParams);
		TestResults[] testResult = establishSingleResult(TestId.NETIO_CONFIG, tInfo.getTestIterations());
		
		VerbosePrint.testStart(System.out, tParams, name(), tInfo.getTestIterations());		

		/*
		 * Start the server thread
		 */
		new NetIOConfigServerThread(PriorityType.HighPriority);

		/*
		 * Create a byte array to send as request to server
		 */
		byte [] request = createClientRequest();
		try {
			clientSocket = new Socket("127.0.0.1", 
					TestFinalDefinitions.NETIO_CONFIG_SVR_PORT);
			clientSocket.setSoTimeout(
					TestFinalDefinitions.NETIO_CONFIG_SOCKET_TIMEOUT);
		} catch (UnknownHostException e1) {
			e1.printStackTrace();
		} catch (IOException e1) {
			e1.printStackTrace();
		}
		try {
			outputStream = clientSocket.getOutputStream();
		} catch (IOException e1) {
			e1.printStackTrace();
		}
		dOutputStream = new DataOutputStream(outputStream);

		try {
			inputStream = clientSocket.getInputStream();
		} catch (IOException e1) {
			e1.printStackTrace();
		}
		dInputStream = new DataInputStream(inputStream);		
		/*
		 * We are good to start off the client operations.
		 */
		while ( true ) {
			if ( count >= tInfo.getThreshold() ) {
				break;
			}
			testResult[0].resetResults();			
			for (int loop = 0; loop < new_iters; loop++) {
				long timeDiff = 0;
				startTime = System.nanoTime();
				for (int i = 0; i < loops; i++) {
					try {
						dOutputStream.write(request);
						dInputStream.read(request);
					} catch (IOException e) {
						e.printStackTrace();
					}					
				}
				endTime = System.nanoTime();
				/* Get the elapsed time in nsecs */

				timeDiff = endTime - startTime;
				VerbosePrint.diffTime(System.out, tParams, name(), timeDiff);					

				testResult[0].setValuesAtNextIndex(timeDiff);
			}
			if (testResult[0].getMedian() > clockAccuracy) {
				retryMsg = true;
				count++;
				/* Check against the input parameters for this test */
				if(testResult[0].checkStdDev(rtValues, tParams)) {
					retryMsg = false;
					break;
				}
			}

			/* Std dev time is not accurate enough, so retry */
			loops *= 2;
			if (retryMsg) {
				VerbosePrint.testRetry(System.out, tParams, name());
			}
		}
		cleanup();
		return testResult;
	}

	private void cleanup() {
		try {
			clientSocket.close();
		} catch (IOException e) {
			e.printStackTrace();
		}
	}
	
	@Override
	public String name() {
		return "NetIO";
	}
		
}

