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
 
#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/ip.h>
#include <errno.h>
#include <sys/time.h>
#include <stdlib.h>
#include <iostream>
#include <string.h>
#include <unistd.h>
#include <list>
#include <assert.h>
#include <netdb.h>
#include <arpa/inet.h>

#include <TestResults.h>
#include <ResultTable.h>
#include <config_test.h>
#include <BenchMarkTestInfo.h>
#include <PthreadWrapper.h>
#include <TimeOperations.h>
#include "TestResultsUtils.h"
#include "VerbosePrint.h"

#include <unistd.h>
#include <fcntl.h>
#include <config_test.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/ip.h>
#include <errno.h>
#include <netdb.h>

void cleanup(serverArgs_t *, pthread_attr_t *, pthread_t, int, char *);
void cleanup(serverArgs_t *, pthread_attr_t *, pthread_t, int, char *, list<
      TestResults *> *, TestResults *);

char *msgAllocInit(char *, int);
void *net_server_impl(void *);
int get_local_address(struct in_addr *local_addr);

/*
 * Name    : msgAllocInit()
 * Desc    : Allocates and inits the msg to be sent over the network.
 * Args    : msg     - pointer to the msg that needs allocation
 *	     msgSize - Size of the msg
 * Returns : msg - after alloc and init, else NULL.
 */

char *msgAllocInit(char *msg, int msgSize) {
   int i;
   /* Alloc the msg buffer */
   msg = (char *) malloc(msgSize);
   if (msg == NULL) {
      cerr << "*** Panic *** malloc() failed in msgAllocInit()" << endl;
      return msg;
   }
   memset(msg, 0, msgSize);
   /* Fill up the buffer with some chars */
   for (i = 0; i < msgSize - 1; i++) {
      msg[i] = 'x';
   }

   return msg;
}

/*
 * gets the first IP address available in the localhost
 */
int get_local_address(struct in_addr *local_addr) {
   char host[MAX_HOST_NAME_LEN];

   int i;
   struct hostent *hp;

   /*
    int rc;
    intentionally commenting this code. for now hard code hostname to "localhost"
    rc = gethostname(host, MAX_HOST_NAME_LEN - 1);

    host[sizeof(host) - 1] = '\0';

    if (rc != 0) {
    return -1;
    }

    */
   strcpy(host, "localhost");
   hp = gethostbyname(host);

   if (!hp) {
      cerr << "error in host lookup: " << h_errno << endl;
      return -1;
   }

   if (hp->h_addrtype == AF_INET && hp->h_addr_list[0]) {
      *local_addr = (*(struct in_addr *) hp->h_addr_list[0]);
      return 0;
   }

   return -1;
}

/*
 * Name    : net_server_impl()
 * Desc    : Server implementation.
 * Args    : args - arguments for the server.
 * Returns : NULL;
 */
void *net_server_impl(void *args) {
   int clientSocket = 0;
   char *rcvMsg = NULL;
   int msgSize = DEF_NETIO_SZ;
   int serverSocket = 0;
   int queueBacklog = 1;
   serverArgs_t *servArgs = (serverArgs_t *) args;
   int ret = 0, retry = 0, opt_val = 1;
   int i = 0;

   struct sockaddr_in clientDet;
   struct sockaddr_in serverDet;
   socklen_t csockLen = sizeof(clientDet);
   struct timeval t;

   rcvMsg = (char *) malloc(msgSize);
   if (rcvMsg == NULL) {
      cerr << "*** Panic *** malloc() failed in net_server_impl()" << endl;
      exit(-1);
   }

   /* Set the params for the server side stuff */
   serverDet.sin_family = AF_INET;
   serverDet.sin_addr.s_addr = INADDR_ANY;
   serverDet.sin_port = htons(SERVER_PORT);
   memset(serverDet.sin_zero, '\0', sizeof(serverDet.sin_zero));

   /* Open server socket */
   serverSocket = socket(AF_INET, SOCK_STREAM, 0);
   if (serverSocket == -1) {
      cerr << "net_serv_impl: Error creating server socket" << endl;
      if (rcvMsg != NULL) {
         free(rcvMsg);
      }
      return (void *) NULL;
   }
   t.tv_sec = 10;
   t.tv_usec = 0;

   if (setsockopt(serverSocket, SOL_SOCKET, SO_RCVTIMEO, &t,
         sizeof(struct timeval)) != 0) {
      perror("net_server_impl");
      cerr << "net_server_impl: Error while setting SO_RCVTIMEO "
         "socket option" << endl;

      if (rcvMsg != NULL) {
         free(rcvMsg);
      }
      if (serverSocket != 0) {
         close(serverSocket);
      }
      return (void *) NULL;
   }

   if (setsockopt(serverSocket, SOL_SOCKET, SO_REUSEADDR, (void *) &opt_val,
         sizeof(int)) != 0) {
      perror("net_server_impl");
      cerr << "net_server_impl: Error while setting SO_REUSEADDR "
            << "socket option." << endl;

      if (rcvMsg != NULL) {
         free(rcvMsg);
      }
      if (serverSocket != 0) {
         close(serverSocket);
      }

      return (void *) NULL;
   }

   /* Bind to the socket */
   if (bind(serverSocket, (struct sockaddr *) &serverDet,
         sizeof(struct sockaddr)) == -1) {
      cerr << "net_server_impl: Error binding to the server socket." << endl;
      perror("net_server_impl");

      if (rcvMsg != NULL) {
         free(rcvMsg);
      }
      if (serverSocket != 0) {
         close(serverSocket);
      }
      return (void *) NULL;
   }
   /* Listen */
   if (listen(serverSocket, queueBacklog) == -1) {
      cerr << "net_server_impl: Error listening to the server socket" << endl;

      if (rcvMsg != NULL) {
         free(rcvMsg);
      }
      if (serverSocket != 0) {
         close(serverSocket);
      }
      return (void *) NULL;
   }
   /* Get into a loop to accept the client connection */
   while (servArgs->stayAlive) {
      /* Accept if client pings */
      if ((clientSocket = accept(serverSocket, (struct sockaddr *) &clientDet,
            &csockLen)) == -1) {
         continue;
      } else {
         break;
      }
   }

   /* Connection is done. So, wait till we recv response */
   while (servArgs->stayAlive) {
      int bytesRemaining;
      char *buffer;

      for (i = 0; i < (servArgs -> iterations) && (servArgs -> stayAlive); i++) {
         bytesRemaining = msgSize;
         buffer = rcvMsg;

         do {
            ret = recv(clientSocket, buffer, bytesRemaining, 0);
            retry = 0;
            if (ret == -1) {
               if (errno == EINTR) {
                  retry = 1;
               }
            } else if (ret > 0 && ret < bytesRemaining) {
               buffer += ret;
               bytesRemaining -= ret;
               retry = 1;
            }
         } while (retry != 0);

         if ((retry == 0) && (ret == -1)) {
            close(clientSocket);
            if (serverSocket != 0) {
               close(serverSocket);
            }

            if (rcvMsg != NULL) {
               free(rcvMsg);
            }
            return (void *) NULL;
         }

         /* Send the reponse back */
         if (send(clientSocket, buffer, msgSize, 0) != msgSize) {
            close(clientSocket);
            if (serverSocket != 0) {
               close(serverSocket);
            }

            if (rcvMsg != NULL) {
               free(rcvMsg);
            }
            return (void *) NULL;
         }
      }

      msgSize *= 2;

      free(rcvMsg);
      rcvMsg = (char *) calloc(sizeof(char), msgSize);

      if (rcvMsg == NULL) {
         cerr << "unable to allocate memory" << endl;
         abort();
      }
   }

   if (rcvMsg != NULL) {
      free(rcvMsg);
   }

   close(clientSocket);
   close(serverSocket);
   return (void *) NULL;
}

/*
 * Name    : net_io_test()
 * Desc    : Determines the net IO rate on the system
 * Args    : 	clParams 	- command line args.
 *		testInfo	- Test information from input file.
 *	     	rtValues 	- Real time values.
 *		resultTable	- Table of results.
 * Returns : NULL;
 */

extern "C" bool net_io_test(CmdLineParams &clParams,
      BenchMarkTestInfo &testInfo, RealTimeValues &rtValues,
      ResultTable &resultTable) {

   /* Local variables of this function */
   int loop = 0, count = 0;
   bool done(false);
   struct timespec startTime, endTime;
   long long timeDiff = 0;
   pthread_t serverTid = 0;
   struct sockaddr_in serverDet;
   int clSockFd = 0;
   int new_iters = testInfo.getIterations();
   pthread_attr_t *attr = NULL;
   serverArgs_t *servArgs = NULL;
   char *msg = NULL;
   int msgSize = DEF_NETIO_SZ;
   bool printFlag(false);
   long long clockAccuracy = getClockAccuracy(resultTable);

   /* Use this to set the timeout value for the socket */
   struct timeval t;

   /*
    * Make sure the client thread is running the right sched policy
    * and priority.
    */
   PthreadWrapper::setSchedParam(pthread_self(), SCHED_FIFO, HIGH_PRIO_VAL);

   /* If we are running quick mode, then we should just do 1 iteration */
   if (clParams.isQuickRun()) {
      new_iters = 1;
   } else {
      new_iters = new_iters * rtValues.getMultiplier();
   }
   testInfo.setIterations(new_iters);

   list<TestResults *> *listOfResults = new list<TestResults *> ();
   TestResults *tResult = new TestResults(NETIO_CONFIG, new_iters);

   /* Keep server thread alive with this stayAlive argument variable */
   servArgs = (serverArgs_t *) malloc(sizeof(serverArgs_t));
   if (servArgs == (serverArgs_t *) NULL) {
      cerr << "net_io_test: malloc() of serverArgs_t failed" << endl;
      cleanup(servArgs, attr, serverTid, clSockFd, msg, listOfResults, tResult);
      return false;
   }
   memset(servArgs, 0, sizeof(serverArgs_t));
   servArgs->stayAlive = true;
   servArgs->iterations = new_iters;

   /* Create pthread attribute for the RT test thread */
   attr = (pthread_attr_t *) malloc(sizeof(pthread_attr_t));
   if (attr == NULL) {
      abort();
   }

   pthread_attr_init(attr);
   PthreadWrapper::setPthreadAttrSchedParam(attr, SCHED_FIFO, HIGH_PRIO_VAL);

   /* Create and start RT server thread */
   if (pthread_create(&serverTid, attr, net_server_impl, (void *) servArgs)
         == -1) {
      cerr << "net_io_test(): Thread creation failed" << endl;
      cleanup(servArgs, attr, serverTid, clSockFd, msg, listOfResults, tResult);
      abort();
   }

   /* Alloc and init the msg buffer */
   msg = (char *) msgAllocInit(msg, msgSize);
   if (msg == NULL) {
      cerr << "net_io_test: malloc() of msg buffer failed" << endl;
      cleanup(servArgs, attr, serverTid, clSockFd, msg, listOfResults, tResult);
      return false;
   }

   /* Set the params for the server side stuff */
   serverDet.sin_family = AF_INET;

   if (get_local_address(&serverDet.sin_addr) != 0) {
      cerr << "unable to get local IP " << endl;
      cleanup(servArgs, attr, serverTid, clSockFd, msg, listOfResults, tResult);
      return false;
   }

   serverDet.sin_port = htons(SERVER_PORT);
   memset(serverDet.sin_zero, '\0', sizeof(serverDet.sin_zero));

   /* Create the socket fd */
   clSockFd = socket(AF_INET, SOCK_STREAM, 0);
   if (clSockFd == -1) {
      cerr << "net_io_test: Error creating client socket" << endl;
      cleanup(servArgs, attr, serverTid, clSockFd, msg, listOfResults, tResult);
      return false;
   }

   /*
    * We do not want the server thread to indefinitely wait. Set
    * timeout to some safe value (10 secs).
    */
   t.tv_sec = 10;
   t.tv_usec = 0;

   if (setsockopt(clSockFd, SOL_SOCKET, SO_RCVTIMEO, (void *) &t,
         sizeof(struct timeval)) != 0) {
      cleanup(servArgs, attr, serverTid, clSockFd, msg, listOfResults, tResult);
      return false;
   }

   /* give some time for the server thread to start */
   sleep(2);
   /* Connect to the server */

   int attempt = 0;
   int maxRetry = 10;

   while (connect(clSockFd, (struct sockaddr *) &serverDet, sizeof(serverDet))
         == -1 && attempt < maxRetry) {

      if (clParams.isVerbose()) {
         if (clParams.getVerboseLevel() >= 1) {
            cout << "net_io_test: " << "Retrying connect "
                  << "to the server socket" << endl;
         }
      }

      TimeOperations::doNanoSleep(MILLI_SECONDS);
      attempt++;
   }

   if (attempt == maxRetry) {
      /*
       * Since we are unable to connect to the server,
       * we cannot proceed further
       */
      cleanup(servArgs, attr, serverTid, clSockFd, msg, listOfResults, tResult);
      abort();
      return false;
   }

   if (clParams.isVerbose() && (clParams.getVerboseLevel() >= 1)) {
      cout << endl << "Test Report for net_io_test test:" << endl;
      cout << "===============================================" << endl;
      cout << endl << "net_io_test: Total number of iterations: " << new_iters
            << endl;
   }

   /* Connection done. Now, try exchanging some msgs with server */
   while (!done) {
      for (loop = 0; loop < new_iters; loop++) {
         int ret = 0, retry = 0;
         int bytesRemaining;
         char* buffer;

         bytesRemaining = msgSize;
         buffer = msg;
         /* Record start time */
         TimeOperations::getCurTime(&startTime);

         if (send(clSockFd, buffer, bytesRemaining, 0) != msgSize) {
            cleanup(servArgs, attr, serverTid, clSockFd, msg, listOfResults,
                  tResult);
            return false;
         }

         /* Wait till you recv response */
         do {
            ret = recv(clSockFd, buffer, bytesRemaining, 0);
            retry = 0;
            if (ret == -1) {
               if (errno == EINTR) {
                  retry = 1;
               }
            } else if (ret > 0 && ret < bytesRemaining) {
               buffer += ret;
               bytesRemaining -= ret;
               retry = 1;
            }
         } while (retry != 0);

         /*  Now, record end time */
         TimeOperations::getCurTime(&endTime);

         /* If there was error other than EINTR, return fail */
         if ((retry == 0) && (ret == -1)) {
            cleanup(servArgs, attr, serverTid, clSockFd, msg, listOfResults,
                  tResult);
            return false;
         }

         /* Get the time difference of start and end times */
         timeDiff = TimeOperations::getTimeDiff(endTime, startTime);
         if (clParams.isVerbose() && (clParams.getVerboseLevel() >= 2)) {
            cout << "net_io_test: " << "Difference between end and "
                  << "start times = " << timeDiff << endl;
         }

         tResult->setValuesAtNextIndex(timeDiff);
      }

      if (tResult -> getMedian() > clockAccuracy) {
         printFlag = true;

         if (tResult->checkStdDev(rtValues.getRangePercentage(),
               rtValues.getAcceptablePercentage(),
               clParams.getVerboseLevel(), false)) {
            done = true;
            break;
         } else {
            /*If we have completed, return error*/
            if (++count == testInfo.getThreshold()) {
               done = true;
               break;
            }

            if (printFlag) {
               VerbosePrint::rtmbPrintRetryMsg(clParams, 1, "net_io");
            }
         }
      }
      /*
       * Std dev time is not accurate enough, so retry with
       * larger buffer.
       */
      delete tResult;
      tResult = new TestResults(NETIO_CONFIG, new_iters);
      msgSize *= 2;

      free(msg);

      msg = (char *) calloc(sizeof(char), msgSize);

      if (msg == NULL) {
         cerr << "unable to allocate "
            "memory";
         abort();
      }

   }

   listOfResults->push_back(tResult);
   resultTable.addResult(testInfo, listOfResults);
   tResult->setWorkQuantum(msgSize);

   fflush(stderr);
   fflush(stdout);
   /* Clean up and leave */

   cleanup(servArgs, attr, serverTid, clSockFd, msg);

   return true;
}

void cleanup(serverArgs_t *servArgs, pthread_attr_t *attr, pthread_t serverTid,
      int clSockFd, char *msg) {

   if (clSockFd != 0) {
      close(clSockFd);
   }

   if (serverTid != 0) {
      servArgs->stayAlive = false;
      pthread_join(serverTid, (void **) NULL);
   }
   if (msg != NULL) {
      free(msg);
   }
   if (servArgs != NULL) {
      free(servArgs);
   }

   if (attr != NULL) {
      pthread_attr_destroy(attr);
      free(attr);
   }
}

/*
 * Name    : cleanup()
 * Desc    : free()s all the memory used by the test.
 * Args    : servArgs  - Arguments passed to server thread.
 *	     server_tid - tid of the server.
 *	     clSockFd - socket fd that was used for communication.
 *	     msg	- message that was being passed around.
 * 	     listOfResults - List of testresults
 * 	     tResult	- Test Result object pointer.
 * Returns : void
 */
void cleanup(serverArgs_t *servArgs, pthread_attr_t *attr, pthread_t serverTid,
      int clSockFd, char *msg, list<TestResults *> *listOfResults,
      TestResults *tResult) {

   if (clSockFd != 0) {
      close(clSockFd);
   }

   if (serverTid != 0) {
      servArgs-> stayAlive = false;
      pthread_join(serverTid, (void **) NULL);
   }

   if (msg != NULL) {
      free(msg);
   }
   if (servArgs != NULL) {
      free(servArgs);
   }

   if (tResult != NULL) {
      delete tResult;
   }

   if (attr != NULL) {
      pthread_attr_destroy(attr);
      free(attr);
   }
}
