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
#define MATRIX_SIZE 5

#include <thread_priority_test.h>

/* Globals */
threadSyncers *tsyncers = NULL;
threadSyncers *parentSema = NULL;
threadId *thrId = NULL;
static int counter = 0;
static CmdLineParams *cmdLineParams;

/*
 * Name     : thread_priority()
 * Desc     : Tests if higher priority threads preempt lower priority threads.
 * Args     : args - Test arguments passed from harness.
 * Returns  : NULL.
 */

void printStopTimes(int threadIndex, thrArgs *args, int onlineCpus,
        CmdLineParamsBase& cmdLineParams) {
	int j;

	TestResults *result = args[0].results;
	long long* values = result -> getValues();

	for( j = 0 ; j < onlineCpus; j++) {
		VerbosePrint::rtmbVerbosePrintlnOut(cmdLineParams, 2,
		        "thread_priority: Stop Time of thread %d= %.3f us",
		        threadIndex + j, MICROSEC(values[j]));
	}
}

extern "C"bool thread_priority(CmdLineParams &clParams,
	BenchMarkTestInfo &testInfo, RealTimeValues &rtValues,
	ResultTable &resultTable) {

	pthread_t tid;
	pthread_attr_t *attr = NULL;
	int newIters = testInfo.getIterations();
	bool rc (true);

	/* If we are running quick mode, then we should just do 1 iteration */
	if (clParams.isQuickRun()) {
		newIters = 1;
	} else {
		newIters = newIters * rtValues.getMultiplier();
	}
	testInfo.setIterations (newIters);

	cmdLineParams = &clParams;

	/* Locals */
	int onlineCpus = 0;
	int i = 0,j=0,k=0;
	thrArgs *lowPrioArgs = NULL, *medPrioArgs = NULL, *highPrioArgs = NULL;
	pthread_attr_t highPrioAttr, lowPrioAttr , medPrioAttr;
	int totalThreads = 0;
	struct timespec waitTime;

	priorityValues[LOW_PRIORITY]=LOW_PRIO_VAL;
	priorityValues[MED_PRIORITY]=MED_PRIO_VAL;
	priorityValues[HIGH_PRIORITY]=HIGH_PRIO_VAL;

	/* Get the total number of online CPUs on the system */
	onlineCpus = getNumberOfOnlineCpus();
	if (onlineCpus == 1) {
		VerbosePrint::rtmbVerbosePrintlnOut( *cmdLineParams , 2 ,
			"thread_priority: "
			"Test not done on uniprocessor systems\n");
		return (void *) NULL;
	}

	totalThreads = onlineCpus * MAX_PRIO_TYPES;

	TestResults *tResult = new TestResults[3];

	for(i = 0; i < 3; i++ ) {
		tResult[i].setTestIds( THREAD_PRIORITY_TEST );
		tResult[i].setNumberOfValues(onlineCpus); /* we will have only one value per thread */
	}

	thrId = (threadId *) calloc (sizeof (threadId) * onlineCpus * MAX_PRIO_TYPES, 1);
	lowPrioArgs = (thrArgs *) calloc (sizeof ( thrArgs) * onlineCpus ,1 );
	medPrioArgs = (thrArgs *) calloc (sizeof ( thrArgs) * onlineCpus ,1);
	highPrioArgs = (thrArgs *) calloc (sizeof ( thrArgs) * onlineCpus ,1);

	if ( !thrId || !lowPrioArgs || !medPrioArgs || !highPrioArgs ) {
		VerbosePrint::rtmbVerbosePrintlnErr( *cmdLineParams , 2 ,
			" malloc failed in thread_priority %s:%d \n" , __FILE__,__LINE__ );
		abort();
	}

	/* Allocate memory for the mutex and cond vars that we use during this test */
	tsyncers = (threadSyncers *) malloc (sizeof (threadSyncers));
	if (tsyncers == NULL) {
		VerbosePrint::rtmbVerbosePrintlnErr( *cmdLineParams , 2 ,
			" malloc failed in thread_priority %s:%d \n" , __FILE__,__LINE__ );
		abort();
	}
	memset (tsyncers, 0, sizeof(threadSyncers));
	pthread_mutex_init (&tsyncers->mutex, NULL);
	pthread_cond_init (&tsyncers->cond, NULL);

	parentSema = (threadSyncers *) malloc (sizeof (threadSyncers));
	if (parentSema == NULL) {
		VerbosePrint::rtmbVerbosePrintlnErr( *cmdLineParams , 2 ,
			" malloc failed in thread_priority %s:%d \n" , __FILE__,__LINE__ );
		abort();
	}
	memset (parentSema, 0, sizeof(threadSyncers));
	pthread_mutex_init (&parentSema->mutex, NULL);
	pthread_cond_init (&parentSema->cond, NULL);

	int workQuantum = getClockWorkQuantum(resultTable);

	if (clParams.isVerbose() && (clParams.getVerboseLevel() >= 1) ) {
		cout << endl << "Test Report for thread_priority test:" << endl;
		cout << " ===============================================" << endl;
		cout << endl << "thread_priority: Total number of iterations: "
		<< newIters << endl;
	}

	VerbosePrint::rtmbVerbosePrintlnOut(*cmdLineParams , 3 , " workQuantum =%d \n" , workQuantum);

	TestResults *iterTimeResult = new TestResults(THREAD_PRIORITY_TEST, newIters);

	iterTimeResult->setNumberOfValues(onlineCpus);

	tResult[0].setTestResultName(" High Priority Thread");
	tResult[1].setTestResultName(" Medium Priority Thread");
	tResult[2].setTestResultName(" Low Priority Thread");

	CreateStartThreads( &highPrioAttr , HIGH_PRIORITY, highPrioArgs,
		&tResult[0], iterTimeResult, workQuantum, onlineCpus, newIters);
	CreateStartThreads( &medPrioAttr , MED_PRIORITY, medPrioArgs,
		&tResult[1], NULL , workQuantum, onlineCpus, newIters);
	CreateStartThreads( &lowPrioAttr , LOW_PRIORITY, lowPrioArgs,
		&tResult[2], NULL , workQuantum, onlineCpus, newIters);

	waitTime.tv_sec = 1;
	waitTime.tv_nsec = 0;

	while (counter != totalThreads) {
		pthread_mutex_lock (&parentSema->mutex);
		pthread_cond_timedwait(&parentSema->cond, &parentSema->mutex,
			&waitTime);
		pthread_mutex_unlock (&parentSema->mutex);
	}
	pthread_mutex_lock (&tsyncers->mutex);
	pthread_cond_broadcast (&tsyncers->cond);
	pthread_mutex_unlock (&tsyncers->mutex);

	/* Parent thread waits for all threads to complete */
	while (counter != 0) {
		pthread_mutex_lock (&parentSema->mutex);
		pthread_cond_wait(&parentSema->cond, &parentSema->mutex);
		pthread_mutex_unlock (&parentSema->mutex);
	}

	for(i = 0; i < totalThreads; i++) {
		pthread_join(thrId[i].tid, NULL);
	}

	for ( i = 0; i < 3;i++ ) {
		for( j = 0; j < onlineCpus; j++ ) {
			VerbosePrint::rtmbVerbosePrintlnOut( *cmdLineParams ,
				3, " id=%d value=%lld \n", i,
				(tResult[i].getValues())[j] );

		}
	}

	/*
	 * Last bit - check if there was any lower priority thread that finished
	 * before the higher priority threads. If yes, then test failed. If no,
	 * then test has passed.
	 */
	bool done = false;

	for ( i = 0; i < 3 && !done; i++ ) {
		long long min = tResult[i].getMin();
		long long max = tResult[i].getMax();

		for( j = i+1; j < 3 && !done; j ++ ) {
			if( min> tResult[j].getMin() || max> tResult[j].getMax() ) {
				VerbosePrint::rtmbVerbosePrintlnErr( *cmdLineParams
					, 1 ,
					" thread_priority: Test Failed \n");
				VerbosePrint::rtmbVerbosePrintlnErr( *cmdLineParams
					, 1 ,
					" Lower priority thread exited before "
					"higher priority thread\n");
				VerbosePrint::rtmbVerbosePrintlnErr( *cmdLineParams
					, 1 ," min = %lld max = %lld "
					"compared with min = %lld max = %lld\n",
					min , max , tResult[j].getMin()
					,tResult[j].getMax());
				rc = false;
				break;
			}
		}
	}

	printStopTimes(0, highPrioArgs, onlineCpus, clParams);
	printStopTimes(onlineCpus, medPrioArgs, onlineCpus, clParams);
	printStopTimes(2 * onlineCpus, lowPrioArgs, onlineCpus, clParams);

	long long *execTime = iterTimeResult -> getValues();
	for(k = 0; k < iterTimeResult -> getNumberOfEntries(); k ++ ) {
		VerbosePrint::rtmbVerbosePrintlnOut( *cmdLineParams , 2 ,
			" thread_priority: Difference between end "
			"and start times = %.3f us", MICROSEC(execTime[k]));
	}

	list<TestResults *> *listOfResults = new list<TestResults *>();
	listOfResults -> push_back(iterTimeResult);

	if( rc ) {
		list <TestScore *> *tScore = generateScores(
			listOfResults,testInfo.getReferenceScore());
		FinalBenchMarkTestResult *fbmt = new FinalBenchMarkTestResult(
			tScore );
		resultTable.addResult(testInfo, fbmt);
	} else {
		TestScore* score = new TestScore(listOfResults);
		FinalBenchMarkTestResult *fbmt = new FinalBenchMarkTestResult(
			listOfResults);
		resultTable.addResult(testInfo, fbmt);
	}

	cleanup (lowPrioArgs, medPrioArgs, highPrioArgs, &lowPrioAttr,
		&medPrioAttr, &highPrioAttr);
	fflush(stderr);
	fflush(stdout);

	return rc;
}

/*
 * Name     : cleanup()
 * Desc     : free()s malloced memory during the test
 * Args     : lowPrioArgs, medPrioArgs and highPrioArgs - thread args.
 *	      medPrioAttr, lowPrioAttr and highPrioAttr - thread priorities.
 * Returns  : void
 */

void cleanup(thrArgs *lowPrioArgs, thrArgs *medPrioArgs, thrArgs *highPrioArgs,
        pthread_attr_t *lowPrioAttr, pthread_attr_t *medPrioAttr,
        pthread_attr_t *highPrioAttr) {
	if (tsyncers != NULL) {
		pthread_mutex_destroy(&tsyncers->mutex);
		pthread_cond_destroy(&tsyncers->cond);
		free(tsyncers);
	}
	if (parentSema != NULL) {
		pthread_mutex_destroy(&parentSema->mutex);
		pthread_cond_destroy(&parentSema->cond);
		free(parentSema);
	}

	pthread_attr_destroy(highPrioAttr);
	pthread_attr_destroy(lowPrioAttr);
	pthread_attr_destroy(medPrioAttr);

	FREE_IF_NOT_NULL(lowPrioArgs);
	FREE_IF_NOT_NULL(medPrioArgs);
	FREE_IF_NOT_NULL(highPrioArgs);
}

/*
 * Name    : CreateStartThreads()
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
int CreateStartThreads(pthread_attr_t *attr, int priority, thrArgs* args,
        TestResults* tResult, TestResults *execTimeResults, int opernAmount,
        int onlineCpus, int iterations) {
	int i;
	static int count = 0;
	pthread_attr_t attr1;
	struct sched_param param;

	int policy = 0;
	int priority_value = priorityValues[priority];

	args -> type = priority;
	args -> attr = attr;
	args -> opernAmount = opernAmount;
	args -> onlineCpus = onlineCpus;
	args -> iterations = iterations;

	pthread_attr_init(attr);
	PthreadWrapper::setPthreadAttrSchedParam(args -> attr,
	        (int) SCHED_FIFO, priority_value);

	for (i = 0; i < args->onlineCpus; i++) {
		//to store the stop time of thread
		args -> results = tResult;
		//to store the time of iteration for high priority thread alone
		args -> execTimeResults = execTimeResults;

		if (pthread_create(&thrId[count++].tid, args->attr, _start,
		        (void *) args) == ERROR) {
			int err = errno;
			VerbosePrint::rtmbVerbosePrintlnErr(*cmdLineParams,
			        2, " errorcode=%d at %s:%d \n", err, __FILE__,
			        __LINE__ );
			return ERROR;
		}
	}

	return SUCCESS;
}

/*
 * Name    : lockIncCounter()
 * Desc    : Increment the thread counter under protection of a lock.
 * Args    : total_thrs = total number of threads of all prios started.
 * Returns : void
 */

/* probably name this function line_threads_up() */
void lockIncCounter(int total_thrs) {
	/* Acquire the lock on the list */
	pthread_mutex_lock(&tsyncers->mutex);

	/* Increment the counter to state "self" was started */
	counter++;

	pthread_cond_wait(&tsyncers->cond, &tsyncers->mutex);

	/* Unlock and let other threads increment */
	pthread_mutex_unlock(&tsyncers->mutex);
}

/*
 * Name    : lockDecCounter()
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
		pthread_mutex_lock(&parentSema->mutex);
		pthread_cond_signal(&parentSema->cond);
		pthread_mutex_unlock(&parentSema->mutex);
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
	thrArgs *targs = (thrArgs *) args;
	int type = targs->type;
	int opernAmount = targs->opernAmount;
	TestResults *results = targs->results;
	TestResults *execTimeResults = targs -> execTimeResults;

	struct timespec startTime, endTime;
	long long endTimeInNanos = 0;
	long long ret = 0;
	int i = 0;
	int total_thrs = targs->onlineCpus * MAX_PRIO_TYPES;
	struct sched_param param;
	int policy = -2;
	Matrix a(MATRIX_SIZE,MATRIX_SIZE);
	Matrix b(MATRIX_SIZE,MATRIX_SIZE);
	Matrix c(MATRIX_SIZE,MATRIX_SIZE);

	if (pthread_getschedparam(pthread_self(), &policy, &param) != 0)

	{
		VerbosePrint::rtmbVerbosePrintlnErr(*cmdLineParams, 1,
		        "unable to get thread policy and priority \n");
	}

	assert ((param.sched_priority == priorityValues[type])
		&& (policy == SCHED_FIFO));
	assert (counter <= total_thrs);
	TimeOperations::getCurTime(&startTime);

	/* Increment the counter with protection and wait for all to start */
	lockIncCounter(total_thrs);


	for(i = 0; i < targs -> iterations; i++)
		Matrix::multiply(a,b,&c);

	TimeOperations::getCurTime(&endTime);
	endTimeInNanos = TimeOperations::convertTimeToNsec(endTime);

	results -> setValuesAtNextIndex(endTimeInNanos);
	long long execTime = TimeOperations::getTimeDiff(endTime, startTime);

	execTime = execTime / (targs -> iterations);

	if (execTimeResults != NULL) {
		execTimeResults -> setValuesAtNextIndex(execTime);
	}

	fflush(stderr);
	fflush(stdout);

	/* Decrement the counter and return */
	lockDecCounter();
	return (void *) NULL;
}
