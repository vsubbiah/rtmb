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
 
#include <thread_oprn.h>
#include <errno.h>
int set_sched_param(pthread_t tid, int policy, int prio) {
	struct sched_param param;
	param.sched_priority = prio;
	if (pthread_setschedparam(tid, policy, &param) != SUCCESS) {
		perror("set_sched_param");
		RTMB_printf(stderr,
		        "set_sched_param: Unable to set scheduling param\n");
		return ERROR;
	} else {
		return SUCCESS;
	}
}

int get_sched_param(pthread_t tid, int *policy, struct sched_param *param) {
	if (pthread_getschedparam(tid, policy, param) != SUCCESS) {
		perror("get_sched_param");
		RTMB_printf(stderr,
		        "get_sched_param: Unable to get scheduling param\n");
		return ERROR;
	} else {
		return SUCCESS;
	}
}

int set_pthreadattr_sched_param(pthread_attr_t *attr, int policy, int prio) {
	int rc = SUCCESS;
	struct sched_param param;

	/* POSIX threads inherit the scheduling policy and priority from the parent thread
	 * by default.
	 * Set inherit attribute to false inorder to ensure that the policy and priority specified in the
	 * argument is being set correctly to the attribute specified in the argument
	 */

	int detS;
	detS = PTHREAD_EXPLICIT_SCHED;
	int n;

	if ((n = pthread_attr_setinheritsched(attr, detS)) != SUCCESS) {
		int err_code = errno;
		fprintf(stderr, " errno = %d n = %d  \n", err_code, n);
		perror("pthread_attr_setinheritsched");
		RTMB_printf(stderr,
		        "pthread_attr_setinheritsched: Unable to  turn off inherit scheduling \n");
		rc = ERROR;
	}

	param.sched_priority = prio;

	if ((n = pthread_attr_setschedparam(attr, &param)) != SUCCESS) {
		int err_code = errno;
		fprintf(stderr, " errno = %d n = %d  \n", err_code, n);

		fprintf(stderr, "errno  = %d ", errno);
		fprintf(stderr, " errcodes = %d  %d %d\n", EINVAL, ENOSYS,
		        ENOTSUP);
		perror("set_pthreadattr_sched_param");
		RTMB_printf(stderr,
		        "set_pthreadattr_sched_param: Unable to set priority attribute\n");
		rc = ERROR;
	}

	if ((n = pthread_attr_setschedpolicy(attr, policy)) != SUCCESS) {
		int err_code = errno;
		fprintf(stderr, " errno = %d n = %d  \n", err_code, n);
		perror("set_pthreadattr_schedpolicy");
		RTMB_printf(stderr,
		        "set_pthreadattr_schedpolicy: Unable to set scheduling policy attribute\n");
		rc = ERROR;
	}

	return rc;
}

int get_pthreadattr_sched_param(const pthread_attr_t *attr, int *policy,
        struct sched_param *param) {
	int rc = SUCCESS;
	if (pthread_attr_getschedparam(attr, param) != SUCCESS) {
		perror("get_pthreadattr_sched_param");
		RTMB_printf(stderr,
		        "get_pthreadattr_sched_param: Unable to get sched priority\n");
		rc = ERROR;
	}
	if (pthread_attr_getschedpolicy(attr, policy) != SUCCESS) {
		perror("get_pthreadattr_sched_param");
		RTMB_printf(stderr,
		        "get_pthreadattr_sched_param: Unable to get scheduling policy attribute\n");
		rc = ERROR;
	}
	return rc;
}

/*
 * Name    : get_num_online_cpus()
 * Desc    : Returns the number of online CPUs on the system.
 * Args    : void
 * Returns : Number of online cpus.
 */
int get_num_online_cpus() {
	return sysconf(_SC_NPROCESSORS_ONLN);
}
