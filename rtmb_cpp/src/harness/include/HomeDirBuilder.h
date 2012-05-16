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
 
#ifndef __HOME_DIR_BUILDER__
#define __HOME_DIR_BUILDER__

#include <string>

using namespace std;

class HomeDirBuilder {
private:
   static string *homeDir;
   static void resolveSymLink(char *);

public:
   static void buildHomeDir(string);
   static string& getHomeDir();
};

#endif /*__HOME_DIR_BUILDER__*/

