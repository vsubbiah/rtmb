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
 
#include <DiskIOReadUtils.h>
#include <string.h>
#include <stdlib.h>
#include <iostream>
#include <assert.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/statvfs.h>
using namespace std;

#define FILE_SIZE 100
static char filename[FILE_SIZE];

DiskIOReadUtils::DiskIOReadUtils(int rbSize_, int num) {
   _readBufSize = rbSize_;
   _numberOfBlocks = num;

   _readBuf = (char *) malloc(_readBufSize * num);
   if (_readBuf != NULL) {
      memset(_readBuf, 0, _readBufSize * num);
   } else {
      abort();
   }

   pid_t pid = getpid();
   sprintf(filename, "/tmp/RTMB_disk_io_%lld", (long long) pid);
   _fp = fopen(filename, "w");
   assert(_fp != NULL);

   setupFileForRead();
   fclose(_fp);
}

void DiskIOReadUtils::setupFileForRead() {
   int i = 0;
   char *str = (char *) malloc(_readBufSize);

   if (str != NULL) {
      memset(str, 0, _readBufSize);
      for (i = 0; i < _readBufSize; i++) {
         str[i] = 'x';
      }

      for (i = 0; i < _numberOfBlocks; i++) {
         int n = fwrite(str, sizeof(char), _readBufSize, _fp);

         if (n != _readBufSize) {
            perror("setupFileForRead");
            abort();
         }
      }

      free(str);
      fflush(_fp);
   } else {
      perror("malloc:");
      abort();
   }
}

void DiskIOReadUtils::openFileForRead() {
   _fp = fopen(filename, "r");
   assert(_fp != NULL);
   setbuf(_fp, NULL);
}

void DiskIOReadUtils::closeFile() {
   if (fclose(_fp) != 0) {
      perror("fclose");
   }

   _fp = NULL;
}

void DiskIOReadUtils::cleanup() {
   if (_fp != NULL) {
      if (fclose(_fp) != 0) {
         perror("cleanup");
      }
   }

   unlink(filename);
   free(_readBuf);
}

int DiskIOReadUtils::getBlockSize() {
   struct statvfs buf;
   int rc;
   const char *path = "/tmp";

   rc = statvfs(path, &buf);

   if (rc != 0) {
      abort();
   }

   return buf.f_bsize;
}
