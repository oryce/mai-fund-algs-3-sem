#pragma once

#include "heap.h"
#include "lib/error.h"

extern const heap_vtable_t LEFTIST_HEAP_VTABLE;

typedef struct leftist_node {
	request_t* value;
	struct leftist_node* left;
	struct leftist_node* right;
	size_t npl;
} leftist_node_t;

typedef struct leftist_heap {
	leftist_node_t* root;
} leftist_heap_t;

leftist_heap_t* leftist_heap_create();

void leftist_heap_destroy(leftist_heap_t* heap);

error_t leftist_heap_insert(leftist_heap_t* heap, request_t* value);

bool leftist_heap_is_empty(const leftist_heap_t* heap);

error_t leftist_heap_pop_max(leftist_heap_t* heap, request_t** output);

error_t leftist_heap_get_max(const leftist_heap_t* heap, request_t** output);

error_t leftist_heap_merge_new(const leftist_heap_t* a, const leftist_heap_t* b,
                               leftist_heap_t** output);

error_t leftist_heap_merge(leftist_heap_t* a, leftist_heap_t* b);
