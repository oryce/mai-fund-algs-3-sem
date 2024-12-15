#pragma once

#include <stdbool.h>
#include <stddef.h>

#include "heap.h"
#include "lib/error.h"

extern const heap_vtable_t BINARY_HEAP_VTABLE;

typedef struct bheap_node {
	request_t* value;
} bheap_node_t;

typedef struct binary_heap {
	bheap_node_t* buffer;
	size_t size;
	size_t capacity;
} binary_heap_t;

binary_heap_t* binary_heap_create();

void binary_heap_destroy(binary_heap_t* heap);

error_t binary_heap_insert(binary_heap_t* heap, request_t* value);

bool binary_heap_is_empty(const binary_heap_t* heap);

error_t binary_heap_pop_max(binary_heap_t* heap, request_t** output);

error_t binary_heap_get_max(const binary_heap_t* heap, request_t** output);

error_t binary_heap_merge_new(const binary_heap_t* a, const binary_heap_t* b,
                              binary_heap_t** out);

error_t binary_heap_merge(binary_heap_t* a, binary_heap_t* b);
