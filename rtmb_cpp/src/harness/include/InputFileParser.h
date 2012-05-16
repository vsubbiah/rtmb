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
 
#ifndef __INP_FILE_PARSER__
#define __INP_FILE_PARSER__

#include <stdio.h>
#include <list>
#include <iostream>
#include <string>

#include <InputParams.h>
#include <BenchMarkTest.h>

using namespace std;

class InputFileParser {
private:

   /** Filename of the input file recvd from cmd parser */
   string _filename;

   /** Returns the value in a key-value pair */
   string getRHSofDelimiter(string, string);

   /** Returns the value in a key-value pair */
   string getLHSofDelimiter(string, string);

public:

   /** Constructor */
   InputFileParser(string filename_);

   /** Input file parser */
   InputParams parse();
};

#endif /* __INP_FILE_PARSER__ */
