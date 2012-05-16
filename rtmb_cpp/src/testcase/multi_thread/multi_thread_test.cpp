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
 
#include <iostream>
#include <list>
#include <pthread.h>
#include <stdio.h>
#include <cstdlib>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#include <CmdLineParamsBase.h>
#include <CmdLineParams.h>
#include <InputParams.h>
#include <RTMB.h>
#include <BenchMarkTestInfo.h>
#include <TestResults.h>
#include <TestResultsUtils.h>
#include <RealTimeValues.h>
#include <ResultTable.h>

#include <PthreadWrapper.h>
#include <TimeOperations.h>
#include <DiskIOReadUtils.h>
#include <VerbosePrint.h>
#include <MathOperations.h>
#include <MatrixOperations.h>
#include <errno.h>
#include <math.h>

#include <multi_thread_test.h>

/* Globals */
static int counter = 0;
static thread_syncers_t *tsyncers = NULL;
static thread_syncers_t *parent_sema = NULL;
static thread_id_t *thr_id = NULL;

bool checkResults(TestResults& result, ResultTable &resultTable,
      ConfigParams& cfgParams, char* id) {
   int verboseLvl = cfgParams.verboseLevel;
   bool isVerbose = cfgParams.isVerbose;
   double rangePct = cfgParams.rangePct;
   double acceptablePct = cfgParams.acceptablePct;

   printResult(&result, "multi_thread", verboseLvl);

   bool rc = result.checkStdDev(rangePct, acceptablePct, verboseLvl,
         false);

   return rc;
}

list<TestScore *> *generateScoresForMultiThread(
      list<TestResults *> *listResult, double referenceScore, int onlineCpus) {
   int size = listResult -> size();
   TestResults **tResult = new TestResults *[size];
   long median = -1;
   list<TestResults *>::iterator iter;
   double product = 1.0;
   double sum = 0.0;

   list<TestScore *> *listScore = new list<TestScore *> ();

   int i = 0;
   for (iter = listResult -> begin(); iter != listResult -> end(); iter++) {
      tResult[i++] = (TestResults *) (*iter);
   }

   TestScore *tScore = generateCombinedScore(tResult, size, referenceScore);
   listScore -> push_back(tScore);
   tScore -> setTestResults(listResult);

   TestResults *result = NULL;
   int n = listResult -> size() - 1;

   iter = listResult -> begin();
   i = 0;

   while (i < n) {
      result = (*iter);
      double median = result->getMedian();
      product *= pow(median, 1.0 / n);
      sum += median;
      i++;
      iter++;
   }

   result = (*iter);
   double parallelism = sum / (onlineCpus * result -> getMedian());
   double interimScore = referenceScore / (product / parallelism);

   /*
    double scoreForSubTest = 1;

    scoreForSubTest *= pow(interimScore, 0.5);
    scoreForSubTest *= pow(tScore->getConsistencyPercent(), 0.5);
    tScore -> scoreForSubTest(scoreForSubTest);
    */

   tScore -> scoreForSubTest(interimScore);
   delete tResult;
   return listScore;
}

/*
 * Name     : multiThread()
 * Desc     : Tests if the multiple threads can run simultaneously
 *doing the same amount of work predictably and consistently
 * Args     : args - Test arguments passed from harness.
 * Returns  : NULL.
 */
extern "C" bool multi_thread(CmdLineParams &clParams,
      BenchMarkTestInfo &testInfo, RealTimeValues &rtValues,
      ResultTable &resultTable) {

   int onlineCpus;
   pthread_t tid;
   pthread_attr_t *attr = NULL;
   int newIters = testInfo.getIterations();
   bool rc(true);

   bool verbose = clParams.isVerbose();
   int verboseLvl = clParams.getVerboseLevel();
   int threshold = testInfo.getThreshold();
   int iterations = newIters;
   double rangePct = rtValues.getRangePercentage();
   double acceptablePct = rtValues.getAcceptablePercentage();

   /* If we are running quick mode, then we should just do 1 iteration */
   if (clParams.isQuickRun()) {
      newIters = 1;
   } else {
      newIters = newIters * rtValues.getMultiplier();
   }
   testInfo.setIterations(newIters);

   /* Locals */
   int i = 0, j = 0;
   thr_args_t *high_prio_args = NULL;
   pthread_attr_t high_prio_attr;
   int totalThrs = 0;
   int totalResults = 0;
   struct timespec waitTime;
   ConfigParams cfgParams;

   /* Get the total number of online CPUs on the system */
   onlineCpus = getNumberOfOnlineCpus();

   if (onlineCpus == 1) {
      VerbosePrint::rtmbVerbosePrintlnOut(clParams, 2,
            "multiThread: Test not done on uniprocessor systems");
      return false;
   }

   if (clParams.isVerbose() && (clParams.getVerboseLevel() >= 1)) {
      cout << endl << "Test Report for multi_thread test:" << endl;
      cout << "===============================================" << endl;
      cout << endl << "multi_thread: Total number of iterations: " << newIters
            << endl;
   }

   /* One thread to be created per CPU for simultaneous execution of matrix
    *  multiplication
    */
   totalThrs = onlineCpus + 1;

   list<TestResults *> *listOfResults = new list<TestResults *> ();

   initializeConfigParams(cfgParams, clParams, rtValues, newIters, 1);

   long long clockAccuracy = getClockAccuracy(resultTable);
   sampleRun(cfgParams, clockAccuracy);

   totalResults = totalThrs + 1;
   TestResults *tResult = new TestResults[totalResults];
   TestResults &consolidatedResult = tResult[totalResults - 1];

   for (i = 0; i < totalResults; i++) {
      char buf[30];
      sprintf(buf, "thread#%d", i);
      tResult[i].setTestIds(MULTI_THREAD_TEST);
      tResult[i].setNumberOfValues(newIters);
      tResult[i].setIterations(newIters);
      tResult[i].setResultId(i);
      tResult[i].setTestResultName(buf);
   }

   bool done(false);
   int attemptNumber = 0;

   while (!done) {
      counter = 0;

      for (i = 0; i < totalThrs; i++)
         tResult[i].removeOldValues();

      bool rc = runMultiThreadTest(totalThrs, tResult, consolidatedResult,
            rtValues, clParams, cfgParams, resultTable);

      if (rc) {
         /* values are within acceptable deviations */
         done = true;
         break;
      } else {
         if (++attemptNumber == threshold) {
            done = true;
            break;
         }
      }

      VerbosePrint::rtmbPrintRetryMsg(clParams, 1, "multi_thread");
   }

   for (i = 0; i < totalThrs; i++) {
      listOfResults -> push_back(&tResult[i]);
   }

   list<TestScore *> *tScore = generateScoresForMultiThread(listOfResults,
         testInfo.getReferenceScore(), onlineCpus);
   FinalBenchMarkTestResult *fbmt = new FinalBenchMarkTestResult(tScore);
   resultTable.addResult(testInfo, fbmt);

   return rc;
}

bool runMultiThreadTest(int totalThrs, TestResults *tResult,
      TestResults &consolidatedResult, RealTimeValues &rtValues,
      CmdLineParamsBase &clParams, ConfigParams &cfgParams,
      ResultTable& resultTable) {
   thr_args_t *high_prio_args = NULL;
   pthread_attr_t high_prio_attr;
   int i;
   bool rc(false);
   struct timespec waitTime;
   /* we need 1 result per thread ==> we need  totalThrs results .
    * We need 1 result for consolidation of mean time of individual
    * threads. Totally we have ( totalThrs +1 ) test results
    */
   int totalResults = totalThrs + 1;

   thr_id = (thread_id_t *) calloc(sizeof(thread_id_t), totalThrs);
   high_prio_args = (thr_args_t *) calloc(sizeof(thr_args_t), totalThrs);

   if (!thr_id || !high_prio_args) {
      VerbosePrint::rtmbVerbosePrintlnErr(clParams, 2,
            " malloc failed in multiThread %s:%d \n", __FILE__, __LINE__);
      abort();
   }

   /* Allocate memory for the mutex and cond vars that we use during
    * this test */
   tsyncers = (thread_syncers_t *) calloc(sizeof(thread_syncers_t), 1);
   if (tsyncers == NULL) {
      VerbosePrint::rtmbVerbosePrintlnErr(clParams, 2,
            " malloc failed in multiThread %s:%d \n", __FILE__, __LINE__);
      abort();
   }
   pthread_mutex_init(&tsyncers->mutex, NULL);
   pthread_cond_init(&tsyncers->cond, NULL);

   parent_sema = (thread_syncers_t *) calloc(sizeof(thread_syncers_t), 1);
   if (parent_sema == NULL) {
      VerbosePrint::rtmbVerbosePrintlnErr(clParams, 2,
            " malloc failed in multiThread %s:%d \n", __FILE__, __LINE__);
      abort();
   }
   pthread_mutex_init(&parent_sema->mutex, NULL);
   pthread_cond_init(&parent_sema->cond, NULL);

   createStartThreads(&high_prio_attr, HIGH_PRIO_VAL, high_prio_args,
         &tResult[0], 0, totalThrs - 1, &cfgParams);
   waitTime.tv_sec = 1;
   waitTime.tv_nsec = 0;

   while (counter != (totalThrs - 1)) {
      pthread_mutex_lock(&parent_sema->mutex);
      pthread_cond_timedwait(&parent_sema->cond, &parent_sema->mutex, &waitTime);
      pthread_mutex_unlock(&parent_sema->mutex);
   }
   pthread_mutex_lock(&tsyncers->mutex);
   pthread_cond_broadcast(&tsyncers->cond);
   pthread_mutex_unlock(&tsyncers->mutex);

   /* Parent thread waits for all threads to complete */
   while (counter != 0) {
      pthread_mutex_lock(&parent_sema->mutex);
      pthread_cond_wait(&parent_sema->cond, &parent_sema->mutex);
      pthread_mutex_unlock(&parent_sema->mutex);
   }

   for (i = 0; i < totalThrs - 1; i++) {
      pthread_join(thr_id[i].tid, NULL);
   }

   createStartThreads(&high_prio_attr, HIGH_PRIO_VAL, high_prio_args,
         &tResult[0], totalThrs - 1, totalThrs, &cfgParams);
   pthread_join(thr_id[totalThrs - 1].tid, NULL);

   consolidatedResult.removeOldValues();
   consolidatedResult.setNumberOfValues(totalThrs);
   consolidatedResult.setTestResultName("summaryResult");

   char threadId[20];

   rc = true;
   for (i = 0; i < totalThrs; i++) {
      sprintf(threadId, "thread id:%d ", i);

      if (checkResults(tResult[i], resultTable, cfgParams, threadId) == false) {
         rc = false;
      }

      consolidatedResult.setValuesAtNextIndex(
            (long long) tResult[i].getMedian());
   }

   if (rc) {
      if (checkResults(consolidatedResult, resultTable, cfgParams,
            "thread id:ALL") == false) {
         rc = false;
      }
   }

   cleanup(high_prio_args, &high_prio_attr);
   fflush(stderr);
   fflush(stdout);
   return rc;
}

/*
 * Name     : cleanup()
 * Desc     : free()s malloced memory during the test
 * Args     :  high_prio_args - thread args.
 *	       high_prio_attr - thread priorities.
 * Returns  : void
 */

void cleanup(thr_args_t *high_prio_args, pthread_attr_t *high_prio_attr) {
   if (tsyncers != NULL) {
      pthread_mutex_destroy(&tsyncers->mutex);
      pthread_cond_destroy(&tsyncers->cond);
      free(tsyncers);
   }
   if (parent_sema != NULL) {
      pthread_mutex_destroy(&parent_sema->mutex);
      pthread_cond_destroy(&parent_sema->cond);
      free(parent_sema);
   }

   if (thr_id != NULL) {
      free(thr_id);
   }

   pthread_attr_destroy(high_prio_attr);

   FREE_IF_NOT_NULL(high_prio_args);
}

/*
 * Name    : createStartThreads()
 * Desc    : Creates and starts the threads required for the test
 * Args    : args -
 *                 1. thread attribute
 *                 2. type ( high /medium /low )
 *                 3. thread argument structure
 *                 4. Result structure
 *                 5. Amount of work to do ( for add numbers)
 *                 6. Number of online cpus
 *                 7. Number of times to call the routine to  add numbers.
 * Returns : 0 on success and -1 on failure.
 */
int createStartThreads(pthread_attr_t *attr, int priority, thr_args_t* targs,
      TestResults* tResult, int startIndex, int endIndex,
      ConfigParams* cfgParams) {
   int i;
   pthread_attr_t attr1;
   struct sched_param param;
   thr_args_t* args;

   int policy = 0;
   pthread_attr_init(attr);
   PthreadWrapper::setPthreadAttrSchedParam(attr, (int) SCHED_FIFO, priority);

   for (i = startIndex; i < endIndex; i++) {
      args = &targs[i];
      args -> type = priority;
      args -> attr = attr;
      args -> cfgParams = cfgParams;
      args -> results = &tResult[i];

      if (pthread_create(&thr_id[i].tid, args->attr, _start, (void *) args)
            == ERROR) {
         int err = errno;
         VerbosePrint::rtmbVerbosePrintlnErr(cfgParams ->cmdParams, 2,
               " errorcode=%d at %s:%d \n", err, __FILE__, __LINE__);
         return ERROR;
      }

   }

   return SUCCESS;
}

/*
 * Name    : lock_inc_counter()
 * Desc    : Increment the thread counter under protection of a lock.
 * Args    : totalThrs = total number of threads of all prios started.
 * Returns : void
 */

/* probably name this function lineThreads_up() */
void lockIncCounter() {
   /* Acquire the lock on the list */
   pthread_mutex_lock(&tsyncers->mutex);

   /* Increment the counter to state "self" was started */
   counter++;

   pthread_cond_wait(&tsyncers->cond, &tsyncers->mutex);

   /* Unlock and let other threads increment */
   pthread_mutex_unlock(&tsyncers->mutex);
}

/*
 * Name    : lock_dec_counter()
 * Desc    : Decrement the thread counter under protection of a lock.
 * Args    : void
 * Returns : void
 */
void lockDecCounter() {
   /* Acquire the lock on the list */
   pthread_mutex_lock(&tsyncers->mutex);

   /* Decrement the counter to state "self" has ended */
   counter--;

   /* Notify mother of all threads who is waiting */
   if (counter == 0) {
      pthread_mutex_lock(&parent_sema->mutex);
      pthread_cond_signal(&parent_sema->cond);
      pthread_mutex_unlock(&parent_sema->mutex);
   }
   /* Unlock and let other threads decrement */
   pthread_mutex_unlock(&tsyncers->mutex);
}

/*
 * Name    : _start()
 * Desc    : Start method for a thread.
 * Args    : args - Thread arguments.
 * Returns : NULL
 */
void *_start(void *args) {
   thr_args_t *targs = (thr_args_t *) args;
   int type = targs->type;
   TestResults *results = targs->results;
   ConfigParams *cfgParams = targs -> cfgParams;
   CmdLineParams *clParams = cfgParams -> cmdParams;
   int onlineCpus = getOnlineCpus();
   Matrix a(10, 10), b(10, 10);

   struct timespec startTime, endTime;
   long long diffTime = 0;
   long long ret = 0;
   int i, j;
   struct sched_param param;
   int policy = -2;
   Matrix *c;

   int resultId = results -> getResultId();

   if (pthread_getschedparam(pthread_self(), &policy, &param) != 0) {
      VerbosePrint::rtmbVerbosePrintlnErr(clParams, 1,
            "unable to get thread policy and priority");
      abort();

   }

   assert ((param.sched_priority == HIGH_PRIO_VAL) &&
         (policy == SCHED_FIFO));

   /* Increment the counter with protection and wait for all to start */
   if (resultId != onlineCpus)
      lockIncCounter();

   c = new Matrix(a.getRows(), b.getCols(), false);

   for (i = 0; i < cfgParams -> iterations; i++) {
      TimeOperations::getCurTime(&startTime);

      for (j = 0; j < cfgParams -> subIterations; j++) {
         Matrix::multiply(a, b, c);
      }

      TimeOperations::getCurTime(&endTime);

      long long diffTime = TimeOperations::getTimeDiff(endTime, startTime);

      results->setValuesAtNextIndex(diffTime);
   }

   fflush(stderr);
   fflush(stdout);

   /* Decrement the counter and return */
   if (resultId != onlineCpus)
      lockDecCounter();
   return (void *) NULL;
}

/*
 * Name    : getOnlineCpus()
 * Desc    : Returns the number of online CPUs on the system.
 * Args    : void
 * Returns : Number of online cpus.
 */
int getOnlineCpus() {
   return sysconf(_SC_NPROCESSORS_ONLN);
}

void sampleRun(ConfigParams& cfgParams, long long clockAccuracy) {
   int i, j;

   int retryNumber = -1;
   bool done(false);
   bool testPass(false);
   struct timespec startTime, endTime;
   TestResults *result = new TestResults(MULTI_THREAD_TEST,
         cfgParams.iterations);
   result->setIterations(cfgParams.iterations);
   result->setNumberOfValues(cfgParams.iterations);
   Matrix a(10, 10), b(10, 10);
   Matrix *c = new Matrix(a.getRows(), b.getCols(), false);

   while (!done) {
      retryNumber++;
      result ->removeOldValues();

      for (i = 0; i < cfgParams.iterations; i++) {

         TimeOperations::getCurTime(&startTime);
         for (j = 0; j < cfgParams.subIterations; j++) {
            Matrix::multiply(a, b, c);
         }
         TimeOperations::getCurTime(&endTime);

         long long diffTime = TimeOperations::getTimeDiff(endTime, startTime);
         result ->setValuesAtNextIndex(diffTime);
      }

      int verboseLvl = cfgParams.verboseLevel;
      bool isVerbose = cfgParams.isVerbose;

      if ((result -> getMedian() > clockAccuracy)) {
         done = true;
         break;
      } else {
         done = false;
         cfgParams.subIterations *= 2;
      }
   }

   delete result;
}
