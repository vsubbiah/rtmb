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
 
#include <string>

#include <CmdLineParams.h>

using namespace std;

class CmdLineParser {

   /** Prints all the command line options set */
   void printSelectedOptions(CmdLineParams);

public:

   /** Parser */
   CmdLineParams parse(int, char **);

   /** Prints list of tests that are supported by this benchmark */
   void print_tests_list();

   /** Prints usage */
   void usage();
};
