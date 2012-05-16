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
 
#ifndef VECTOR_H_
#define VECTOR_H_

/* forward referencing */
struct vector_node;
typedef struct vector_node vector_node_t;

struct vector_node {
	void *data;
	vector_node_t *next;
};

typedef struct vector {
	int size;
	vector_node_t *head;
	vector_node_t *last;
} vector_t;

int add_element(vector_t *v, void * ptr);
void free_elements(vector_t *v);
vector_t *create_vector(void);
vector_t *create_vector2(void *data);
void free_elements(vector_t *v);
void *get_element_at(vector_t* v, int index);
void initialize_vector(vector_t *v);

#endif /* VECTOR_H_ */
