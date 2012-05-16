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
 
#ifndef _VERBOSE_PRINT_H
#define _VERBOSE_PRINT_H
#include <CmdLineParamsBase.h>

class VerbosePrint {
private:
   VerbosePrint();

public:
   static void rtmbVerbosePrintlnOut(CmdLineParamsBase &, int verboseLevel,
         const char *, ...);
   static void rtmbVerbosePrintlnErr(CmdLineParamsBase &, int verboseLevel,
         const char *, ...);
   static void rtmbVerbosePrintlnOut(CmdLineParamsBase *, int verboseLevel,
         const char *, ...);
   static void rtmbVerbosePrintlnErr(CmdLineParamsBase *, int verboseLevel,
         const char *, ...);
   static void rtmbVerbosePrintlnErr(const char *msg, ...);
   static void rtmbVerbosePrintlnOut(int requestingVerboseLvl,
         int cmdLineVerboseLvl, const char *msg, ...);
   static void rtmbVerbosePrintlnOut(int cmdLineVerboseLvl, const char *msg,
         ...);
   static void rtmbPrintRetryMsg(CmdLineParamsBase&, int requestingVerboseLvl,
         const char *test_name);
};
#endif

