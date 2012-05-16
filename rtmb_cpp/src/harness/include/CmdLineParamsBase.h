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
 
#ifndef __CMD_LINE_PARAM_BASE__
#define __CMD_LINE_PARAM_BASE__

#include <string>
using namespace std;

class CmdLineParamsBase {

public:

   virtual bool isVerbose(void)=0;
   virtual int getVerboseLevel(void)=0;
   virtual bool isHardRealTime(void)=0;
   virtual bool isQuickRun(void)=0;
   virtual string& getReportDir()=0;
   virtual string& getRTMBBinDir()=0;
};

#endif /*__CMD_LINE_PARAM_BASE__*/
