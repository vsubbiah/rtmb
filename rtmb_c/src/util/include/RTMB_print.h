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
 
#ifndef __RTMB_PRINT__
#define __RTMB_PRINT__

/* System headers are below */
#include <stdio.h>
#include <stdarg.h>

/* RTMB headers are below */
#include <RTMB_rd_cmd_line.h>

/* Func declarations */

int RTMB_printf (FILE *, const char *, ...);
int RTMB_verbose_printf (FILE *, cmd_line_args *, int, const char *, ...);

#endif /* __RTMB_PRINT__ */
