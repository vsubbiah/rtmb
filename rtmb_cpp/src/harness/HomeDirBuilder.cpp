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
#include <iostream>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <sys/param.h>
#include <libgen.h>
#include <stdlib.h>

#include <HomeDirBuilder.h>

using namespace std;

string *HomeDirBuilder::homeDir = new string(" ");

/**
 * Name    : buildHomeDir()
 * Desc    : Builds the home dir from the argv[0]
 * Args    : argv[0]
 * Returns : void
 */

void HomeDirBuilder::buildHomeDir(string fileName_) {
   char *path;
   char *p;
   char *p1;
   int len;
   char *found = NULL;
   struct stat s;
   mode_t mode;

   char *dir = (char *) malloc(MAXPATHLEN);

   // The binary name can be fully or partially qualified

   if ((fileName_.find('/', 0) != string::npos) && (stat(fileName_.c_str(), &s)
         == 0)) {

      // Is this an absolute pathname to the binary ?
      if (fileName_[0] == '/') {
         fileName_.copy(dir, fileName_.size(), 0);
         found = dir;
      } else {
         getcwd(dir, MAXPATHLEN);
         path = dir;
         p = (char *) fileName_.c_str();
         while (p[0] == '.') {
            if (strstr(p, "../") == p) {
               p = p + 3;
               p1 = (char *) dirname(path);
               if (path != p1)
                  strcpy(path, p1);
            } else {
               p = p + 2;
            }
         }
         strcat(path, "/");
         strcat(path, p);
         found = path;
      }
   } else {

      /* If not found, assume it's a short name and search the path variable */
      path = getenv("PATH");
      if (path != NULL) {
         while (path != NULL) {
            /* Get the first element of the path */
            p = strchr(path, ':');
            if (p != NULL) {

               /* There is more than one */
               len = p - path;
               strncpy(dir, path, len);
               dir[len] = 0;
               path = p + 1;
            } else {

               /* Last path element.*/
               strcpy(dir, path);
               path = NULL;
            }

            /* Add the binary name to the path element and check if it
             * points to a file.
             */
            strcat(dir, "/");
            strcat(dir, fileName_.c_str());
            if (stat(dir, &s) == 0) {
               mode = s.st_mode & S_IFMT;
               if (mode == S_IFREG) {
                  /* Found a match! */
                  found = dir;
                  break;
               }
            }
         }
      } else {
         cerr << "ERROR: getenv() failed in buildHomeDir()" << endl;
      }
   }

   /* Found? */
   if (found != NULL) {
      /* Resolve symbolic link in name */
      HomeDirBuilder::resolveSymLink(found);

      /* Strip the exc's name off the end. */
      p = (char *) dirname(found);
      if (found != p) {
         strcpy(found, p);
      }
      HomeDirBuilder::resolveSymLink(found);

      /* Remove the next component which should be '/bin'*/
      p = (char *) dirname(found);
      if (found != p) {
         strcpy(found, p);
      }
      HomeDirBuilder::resolveSymLink(found);

      /* Set it up in the class variable */
      HomeDirBuilder::homeDir = new string(strdup(found));
   }
}

/**
 * Name    : HomeDirBuilder::resolveSymLink()
 * Desc    : Resolves to the actual file if a sym link is sent as argument.
 * Args    : file - char * pointing to the path to the file.
 * Returns : void
 */

void HomeDirBuilder::resolveSymLink(char *file_) {
   char buffer[MAXPATHLEN];
   char *p = file_;
   if (realpath(file_, buffer)) {
      strcpy(p, buffer);
   }
}

string& HomeDirBuilder::getHomeDir() {
   return *(HomeDirBuilder::homeDir);
}
