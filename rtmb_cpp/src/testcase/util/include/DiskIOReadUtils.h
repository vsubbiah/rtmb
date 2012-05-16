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
 
#ifndef __DISK_IO_RD_UTILS__
#define __DISK_IO_RD_UTILS__

#include <stdio.h>

class DiskIOReadUtils {
private:
	int _readBufSize;
	FILE *_fp;
	char *_readBuf;
	int _numberOfBlocks;
public:
	DiskIOReadUtils(int,int);

	inline int doRead() {
		return fread(_readBuf, sizeof(char), _readBufSize, _fp);
	}

	void openFileForRead();
	void closeFile();
	void cleanup();
	void setupFileForRead();
	static int getBlockSize();
};

#endif /*__DISK_IO_RD_UTILS__*/
