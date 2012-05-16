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
 
#include <VerbosePrint.h>
#include <stdio.h>
#include <stdarg.h>

void VerbosePrint::rtmbVerbosePrintlnOut(CmdLineParamsBase &cmdLineArgs_,
      int verboseLevel, const char *msg, ...) {
   int ret;
   va_list args;
   if (cmdLineArgs_.getVerboseLevel() >= verboseLevel) {
      va_start(args, msg);
      ret = vfprintf(stdout, msg, args);
      fprintf(stdout, "\n");
      fflush(stdout);
      va_end(args);
   }
}

void VerbosePrint::rtmbVerbosePrintlnErr(CmdLineParamsBase &cmdLineArgs_,
      int verboseLevel, const char *msg, ...) {
   int ret;
   va_list args;
   if (cmdLineArgs_.getVerboseLevel() >= verboseLevel) {
      va_start(args, msg);
      ret = vfprintf(stderr, msg, args);
      fprintf(stderr, "\n");
      fflush(stderr);
      va_end(args);
   }
}

void VerbosePrint::rtmbVerbosePrintlnOut(CmdLineParamsBase* cmdLineArgs_,
      int verboseLevel, const char *msg, ...) {
   int ret;
   va_list args;
   if (cmdLineArgs_->getVerboseLevel() >= verboseLevel) {
      va_start(args, msg);
      ret = vfprintf(stdout, msg, args);
      fprintf(stdout, "\n");
      fflush(stdout);
      va_end(args);
   }
}

void VerbosePrint::rtmbVerbosePrintlnOut(int requestingVerboseLvl,
      int cmdLineVerboseLvl, const char *msg, ...) {
   int ret;
   va_list args;

   if (requestingVerboseLvl <= cmdLineVerboseLvl) {
      va_start(args, msg);
      ret = vfprintf(stdout, msg, args);
      fprintf(stdout, "\n");
      fflush(stdout);
      va_end(args);
   }
}

void VerbosePrint::rtmbVerbosePrintlnErr(CmdLineParamsBase* cmdLineArgs_,
      int verboseLevel, const char *msg, ...) {
   int ret;
   va_list args;
   if (cmdLineArgs_->getVerboseLevel() >= verboseLevel) {
      va_start(args, msg);
      ret = vfprintf(stderr, msg, args);
      fprintf(stderr, "\n");
      fflush(stderr);
      va_end(args);
   }
}

void VerbosePrint::rtmbVerbosePrintlnErr(const char *msg, ...) {
   int ret;
   va_list args;
   va_start(args, msg);
   ret = vfprintf(stderr, msg, args);
   fflush(stderr);
   va_end(args);

}

void VerbosePrint::rtmbPrintRetryMsg(CmdLineParamsBase& cmdLineArgs,
      int verboseLevel, const char *test_name) {
   if (cmdLineArgs.getVerboseLevel() >= verboseLevel) {
      fprintf(stdout, "\n%s: %s\n", test_name, "Retrying test"
         " with bigger work quantum to reduce variance...\n");
      fflush(stdout);
   }
}
