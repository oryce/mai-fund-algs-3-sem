#pragma once

#include "heap.h"
#include "lib/error.h"

extern const heap_vtable_t SKEW_HEAP_VTABLE;

typedef struct skew_node {
	request_t* value;
	struct skew_node* left;
	struct skew_node* right;
} skew_node_t;

typedef struct skew_heap {
	skew_node_t* root;
} skew_heap_t;

skew_heap_t* skew_heap_create();

void skew_heap_destroy(skew_heap_t* heap);

error_t skew_heap_insert(skew_heap_t* heap, request_t* value);

bool skew_heap_is_empty(const skew_heap_t* heap);

error_t skew_heap_pop_max(skew_heap_t* heap, request_t** output);

error_t skew_heap_get_max(const skew_heap_t* heap, request_t** output);

error_t skew_heap_merge_new(const skew_heap_t* a, const skew_heap_t* b,
                            skew_heap_t** output);

error_t skew_heap_merge(skew_heap_t* a, skew_heap_t* b);
