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
 
#include <RTMB_print.h>

int RTMB_verbose_printf (FILE *fp, cmd_line_args *cmdargs, int verbose_lvl, 
				const char *msg, ...) {
	int ret;
	va_list args;
	if (cmdargs->_verbose == TRUE) {
		if (cmdargs->_verbose_lvl >= verbose_lvl) {
			va_start (args,msg);
			ret = vfprintf (fp, msg, args);
			fflush (fp);
			va_end(args);
		}
	}
	return ret;
}

int RTMB_printf (FILE *fp, const char *msg, ...) {
	int ret;
	va_list args;
	va_start (args,msg);
	ret = vfprintf (fp, msg, args);
	fflush (fp);
	va_end(args);
	return ret;
}
