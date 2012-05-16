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
 
#include "io_opern.h"
#include <sys/types.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <sys/statvfs.h>
#include <RTMB_print.h>

/*
 * Name : open_file_for_read()
 * Desc : Opens a temporary file for read
 * Args : fp - tmp file pointer.
 * Rets : 0 on success and -1 on error.
 */

#define FILE_SIZE 200

static char disk_file[FILE_SIZE];

void disk_io_init() {
	pid_t pid = getpid();
	sprintf(disk_file, "/tmp/RTMB_disk_io_%lld", (long long) pid);
}

int open_file_for_read(FILE **fp) {
	FILE *_fp = fopen(disk_file, "r");

	if (!_fp) {
		perror("fopen:");
		abort();
	}

	*fp = _fp;

	return SUCCESS;
}

/*
 * Name : open_file_for_write()
 * Desc : Opens a temporary file for read
 * Args : fp - tmp file pointer.
 * Rets : 0 on success and -1 on error.
 */
int open_file_for_write(FILE **fp) {
	FILE *_fp = fopen(disk_file, "w");

	if (!_fp) {
		perror("fopen:");
		abort();
	}

	*fp = _fp;

	return SUCCESS;
}

int setup_file_for_read(int loops, int block_size) {
	/* Create and open the new file */
	FILE *_fp = fopen(disk_file, "w");
	int i = 0;

	assert(_fp != NULL );
	char * _read_buf = (char *) malloc(block_size);
	if (_read_buf != NULL) {
		memset(_read_buf, 0, block_size);
	} else {
		abort();
	}

	for (i = loops; i > 0; i--) {
		int n = fwrite(_read_buf, sizeof(char), block_size, _fp);
		if (n != block_size) {
			perror("setupFileForRead");
			abort();
		}
	}

	fclose(_fp);
	return SUCCESS;
}

void disk_io_cleanup() {
	unlink(disk_file);
}

int get_block_size() {
	struct statvfs buf;
	int rc;
	char *path = "/home";

	rc = statvfs(path, &buf);

	if( rc != 0) {
		abort();
	}

	return buf.f_bsize;
}
