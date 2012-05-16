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
 
#ifndef __ARRAY_OPRN__
#define __ARRAY_OPRN__

#include <RTMB_print.h>

#define RTMB_PAGE_SIZE (1024 * 4)
#define SUCCESS 0
#define ERROR -1
#define MATRIX_SIZE 3
#define DEF_ARRAY_SIZE 4 * 1024 * 1024

int do_array_opern(char *m, int size);
void do_matrix_mul();

#endif /*__ARRAY_OPRN__*/
