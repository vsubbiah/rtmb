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
 
#include <stdlib.h>
#include <stdio.h>
#include "vector.h"

int add_element(vector_t *v, void * data) {
	vector_node_t * node = (vector_node_t *) calloc(1,
	        sizeof(vector_node_t));
	int retval = 0;

	if (node != NULL) {
		node -> next = NULL;
		node -> data = data;

		if (!v -> head) {
			v -> head = v -> last = node;
		} else {
			v -> last -> next = node;
			v -> last = node;
		}

		retval = 0;
		v ->size++;
	} else {
		retval = 1;
	}

	return retval;
}

vector_t *create_vector() {
	vector_t * v = (vector_t *) calloc(1, sizeof(vector_t));

	if (v != NULL) {
		initialize_vector(v);
	}

	return v;
}

vector_t *create_vector2(void *data) {
	vector_t * v = (vector_t *) calloc(1, sizeof(vector_t));

	if (v != NULL) {
		initialize_vector(v);
		add_element(v, data);
	}

	return v;
}

void free_elements(vector_t *v) {
	vector_node_t * temp;
	vector_node_t * node = v -> head;

	while (node) {
		temp = node -> next;
		free(node);
		node = temp;
	}
}

void *get_element_at(vector_t* v, int index) {
	void *ptr = NULL;
	vector_node_t* node;

	if (index < v -> size) {
		int i = 0;

		node = v -> head;
		for (i = 0; i < index; i++) {
			node = (vector_node_t *) node -> next;
		}

		ptr = node -> data;
	} else {
		ptr = NULL;
	}

	return ptr;
}

void initialize_vector(vector_t *v) {
	v -> head = NULL;
	v -> last = NULL;
	v -> size = 0;
}
