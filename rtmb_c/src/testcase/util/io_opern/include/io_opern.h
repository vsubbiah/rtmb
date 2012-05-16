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
 
#ifndef __IO_OPERN__
#define __IO_OPERN__

#include <string.h>
#include <stdio.h>

#ifndef ERROR
#define ERROR -1
#endif /*ERROR*/

#ifndef SUCCESS
#define SUCCESS 0
#endif /*SUCCESS*/

void disk_io_init();

int open_file_for_read(FILE **fp);
int open_file_for_write(FILE **fp);
int setup_file_for_read(int loops, int block_size);
void disk_io_cleanup();
int get_block_size();

#endif /*__IO_OPERN__*/
