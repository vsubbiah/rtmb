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
 
#ifndef __CONFIG_TEST__
#define __CONFIG_TEST__

#include <TestResults.h>
#include <CmdLineParams.h>

#define DEF_SZ_CLK_CONF 100
#define NANOSLEEP_TIME  2000
#define DEF_DISKIO_SZ 512
#define DEF_NETIO_SZ 512
#define SERVER_PORT 1703
#define INTRATE_CONFIG_NUM 100;
#define FLTRATE_CONFIG_NUM 100;
#define MCACHE_CONFIG_ARR_SIZE 4 * 1024 * 1024;
#define NETIO_CONFIG_SOCKET_TIMEOUT 10000; // in ms
#define DISKIO_CONFIG_BSIZE 8192

#define CONFIG_RANGE_PCT 20
#define CONFIG_ACCEPTABLE_PCT 99.9

#define HIGHEST_PRIO_VAL (PthreadWrapper::schedGetPriorityMax(SCHED_FIFO))
#define HIGH_PRIO_VAL (HIGHEST_PRIO_VAL - 10)
#define MAX_HOST_NAME_LEN 200
#define MILLI_SECONDS 1000000LL
#define DEFAULT_SUBITERS 2

class RTThreadArgs {
public:
   RTThreadArgs(CmdLineParams& cmdLineParams, int iters, int thresh,
         TestResults* tr, RealTimeValues& rtValues, int clockAcc) :
      clParams(cmdLineParams) {
      /* Now ready the arguments to RT thread that will run the test */

      threshold = thresh;
      iterations = iters;
      tResult = tr;
      rangePct = rtValues.getRangePercentage();
      acceptablePct = rtValues.getAcceptablePercentage();
      clockAccuracy = clockAcc;
      ret = true;

      if (rangePct > CONFIG_RANGE_PCT) {
         rangePct = CONFIG_RANGE_PCT;
      }

      if (acceptablePct < CONFIG_ACCEPTABLE_PCT) {
         acceptablePct = CONFIG_ACCEPTABLE_PCT;
      }
   }

   int threshold;
   int iterations;
   TestResults *tResult;
   CmdLineParams &clParams;
   float rangePct;
   float acceptablePct;
   long long clockAccuracy;
   bool ret;
};

typedef struct serverArgs {
   bool stayAlive;
   int iterations;
} serverArgs_t;

#endif /*__CONFIG_TEST__*/
