#pragma once

#include "heap.h"
#include "lib/error.h"

extern const heap_vtable_t FIBONACCI_HEAP_VTABLE;

typedef struct fibonacci_node {
	request_t* value;
	struct fibonacci_node* child;
	struct fibonacci_node* left;
	struct fibonacci_node* right;
	unsigned degree;
} fibonacci_node_t;

typedef struct fibonacci_heap {
	fibonacci_node_t* max;
	/** Required for the consolidation procedure. */
	size_t size;
} fibonacci_heap_t;

fibonacci_heap_t* fibonacci_heap_create();

void fibonacci_heap_destroy(fibonacci_heap_t* heap);

error_t fibonacci_heap_insert(fibonacci_heap_t* heap, request_t* value);

bool fibonacci_heap_is_empty(const fibonacci_heap_t* heap);

error_t fibonacci_heap_pop_max(fibonacci_heap_t* heap, request_t** output);

error_t fibonacci_heap_get_max(const fibonacci_heap_t* heap,
                               request_t** output);

error_t fibonacci_heap_merge_new(const fibonacci_heap_t* a,
                                 const fibonacci_heap_t* b,
                                 fibonacci_heap_t** output);

error_t fibonacci_heap_merge(fibonacci_heap_t* a, fibonacci_heap_t* b);
