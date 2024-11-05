#pragma once

#include <stdio.h>

#include "lib/collections/string.h"

typedef struct node {
	string_t value;
	struct node* sibling;
	struct node* child;
} node_t;

node_t* node_create(void);

void node_destroy(node_t* node);

void node_print(FILE* fp, node_t* node);

node_t* tree_from_expr(const char* expr);
