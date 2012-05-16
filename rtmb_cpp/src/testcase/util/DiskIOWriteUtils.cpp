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
 
#include <DiskIOWriteUtils.h>
#include <string.h>
#include <stdlib.h>
#include <assert.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/statvfs.h>

#define FILE_SIZE 100
static char filename[FILE_SIZE];

DiskIOWriteUtils::DiskIOWriteUtils(int rbSize_) {
   _writeBufSize = rbSize_;

   _writeBuf = (char *) malloc(_writeBufSize);

   if (_writeBuf != NULL) {
      memset(_writeBuf, 0, _writeBufSize);

      for (int i = 0; i < _writeBufSize; i++) {
         _writeBuf[i] = 'x';
      }
   } else {
      abort();
   }
}

void DiskIOWriteUtils::openFileForWrite() {
   pid_t pid = getpid();
   sprintf(filename, "/tmp/RTMB_disk_io_%lld", (long long) pid);

   _fp = fopen(filename, "w");

   assert(_fp != NULL);
   setbuf(_fp, NULL);
}

void DiskIOWriteUtils::closeFile() {
   if (fclose(_fp) != 0) {
      perror("cleanup");
   }

   _fp = NULL;
}

void DiskIOWriteUtils::cleanup() {
   if (_fp != NULL) {
      if (fclose(_fp) != 0) {
         perror("cleanup");
      }
   }

   unlink(filename);
   free(_writeBuf);
}

int DiskIOWriteUtils::getBlockSize() {
   struct statvfs buf;
   int rc;
   const char *path = "/tmp";

   rc = statvfs(path, &buf);

   if (rc != 0) {
      abort();
   }

   return buf.f_bsize;
}
