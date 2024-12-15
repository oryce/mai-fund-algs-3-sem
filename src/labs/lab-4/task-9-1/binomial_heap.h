#pragma once

#include "heap.h"
#include "lib/error.h"

extern const heap_vtable_t BINOMIAL_HEAP_VTABLE;

typedef struct binomial_tree {
	request_t* value;
	struct binomial_tree* child;
	struct binomial_tree* sibling;
	unsigned degree;
} binomial_tree_t;

typedef struct binomial_heap {
	binomial_tree_t* root;
} binomial_heap_t;

binomial_heap_t* binomial_heap_create();

void binomial_heap_destroy(binomial_heap_t* heap);

error_t binomial_heap_insert(binomial_heap_t* heap, request_t* value);

bool binomial_heap_is_empty(const binomial_heap_t* heap);

error_t binomial_heap_pop_max(binomial_heap_t* heap, request_t** output);

error_t binomial_heap_get_max(const binomial_heap_t* heap, request_t** output);

error_t binomial_heap_merge_new(const binomial_heap_t* a,
                                const binomial_heap_t* b,
                                binomial_heap_t** output);

error_t binomial_heap_merge(binomial_heap_t* a, binomial_heap_t* b);
