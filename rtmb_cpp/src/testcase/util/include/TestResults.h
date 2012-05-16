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
 
#ifndef __TEST_RESULTS__
#define __TEST_RESULTS__

#include <pthread.h>
#include <RealTimeValues.h>
#include <CmdLineParams.h>
#include <assert.h>

#define MULTIPLE_SIMILAR_TESTS 1
#define SINGLE_SUBTEST 2

#define CHECK_VERBOSE_START(verbose,testVerboseLvl,verboseLvl) \
        if (verbose) {                                          \
                if (testVerboseLvl >= verboseLvl) {

#define CHECK_VERBOSE_END                                       \
                }                                               \
        }

#define MICROSEC(nanosecs) (nanosecs/1000.0)

/* precision of 3 decimal places */
#define PRECISION_3 setprecision(3)

/*
 * Changes to TestIds and TestNames must be kept synchronized.
 * TestIds are used to index into TestNames
 */
typedef enum TestIds {
	SYSCLOCK_CONFIG,
	SYSTIMER_CONFIG,
	IRATE_CONFIG,
	FRATE_CONFIG,
	MCACHE_CONFIG,
	DISKIOREAD_CONFIG,
	DISKIOWRITE_CONFIG,
	NETIO_CONFIG,
	TIMER_ACCURACY_TEST,
	EVENT_DISPATCH_TEST,
	THREAD_PRIORITY_TEST,
	LOCK_CONSISTENCY_TEST,
	PERIODIC_EVENTS_TEST,
	ASYNC_EVENTS_TEST,
	DISK_IO_TEST,
	NET_IO_TEST,
	MULTI_THREAD_TEST,
	MULTI_CPU_TEST
} testIds;

static const char* TestNames[] = {
   "clock_config", "timer_config", "int_rate_config", "float_rate_config", "disk_read_config", "disk_write_config",
   "timer_accuracy_test", "event_dispatch_test", "thread_priority_test", "lock_consistency_test", "periodic_events_test",
   "async_events_test", "disk_io_test", "net_io_test", "multi_thread_test", "multi_cpu_test"
};

class TestResults {
private:
	testIds _testId;
	long long _mean;
	long long _min;
	long long _max;
	double _median;
	double _stdDev;
	bool _minUpdated;
	int _entries;
	int _iterations;
	int _maxEntries;
	long long *_values;
	int _workQuantum;
	int _resultId;
	const char* testResultName;
	bool testPass;
	int outlierCount;
	double rangePercent;
	double acceptablePercent;
	int nonConformingIterations;
	int conformingIterations;
	double positiveLimit;
	double negativeLimit;
	void setMinMaxValues();
	void setMin();
	void setMax();
	void setMedian();
public:
	TestResults(testIds, int);
	TestResults();
	~TestResults();
	int getWorkQuantum();
	void setWorkQuantum(int);
	int getResultId();
	void setResultId(int);
	long long getMax();
	long long getMin();
	long long getMean();
	double getMedian();
	void setStdDev();
	void setTestIds(testIds);
	void setNumberOfValues(int);
	void setIterations(int);
	int getIterations();
	void setNumberOfEntries(int num);
	int getNumberOfEntries();
	void removeOldValues();
	double getStdDev();
	long long *getValues();
	testIds getTestId();
	void setValuesAtNextIndex(long long);
	bool checkStdDev(double, double, int, bool);
	string getTestResultName() const;
	void setTestResultName(const char *testResultName);
	bool getTestPass() const;
	void setTestPass(bool testPass);
	int normalizeResults(long long baseVal, long long normalizedVal);
	double getRangePercent() const;
	void setRangePercent(double rangePercent);
	double getAcceptablePercent() const;
	void setAcceptablePercent(double acceptablePercent);
	int getNonConformingIterations() const;
	void setNonConformingIterations(int nonConformingIterations);
	int getConformingIterations() const;
	void setConformingIterations(int conformingIterations);
	double getPositiveLimit() const;
	void setPositiveLimit(double positiveLimit);
	double getNegativeLimit() const;
	void setNegativeLimit(double negativeLimit);
	int getNumberOfValues();
	void cloneValues( TestResults *src );
};

#endif /*__TEST_RESULTS__*/
