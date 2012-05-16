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
 
#ifndef __DISK_IO_WR_UTILS__
#define __DISK_IO_WR_UTILS__

#include <stdio.h>

class DiskIOWriteUtils {
private:
	int _writeBufSize;
	FILE *_fp;
	char *_writeBuf;
public:
	DiskIOWriteUtils(int);

	inline int doWrite() {
		return fwrite(_writeBuf, sizeof(char), _writeBufSize, _fp);
	}

	void cleanup();

	void closeFile();
	void openFileForWrite();
	static int getBlockSize();
};

#endif /*__DISK_IO_WR_UTILS__*/
