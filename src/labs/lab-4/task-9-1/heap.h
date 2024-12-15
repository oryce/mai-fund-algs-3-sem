#pragma once

#include <stdbool.h>

#include "lib/error.h"
#include "request.h"

#define ERROR_HEAP_EMPTY 0x10000001
#define ERROR_HEAP_INCOMPATIBLE 0x10000002

typedef struct heap_vtable {
	/** Creates a new heap. */
	void* (*create)(void);
	/** Destroys the created heap. */
	void (*destroy)(void* heap);
	/** Inserts a value into the heap given a priority number. */
	error_t (*insert)(void* heap, request_t* value);
	/** Returns true if the heap is empty. */
	bool (*is_empty)(const void* heap);
	/** Extracts the maximum element from the heap. */
	error_t (*pop_max)(void* heap, request_t** output);
	/** Returns the maximum element of the heap. */
	error_t (*get_max)(const void* heap, request_t** output);
	/** Merges two heaps together into a new heap. */
	error_t (*merge_new)(const void* heapA, const void* heapB, void** heapOut);
	/** Merges heap B into heap A, destroying heap B. */
	error_t (*merge)(void* heapA, void* heapB);
} heap_vtable_t;

extern const heap_vtable_t* HEAP_VTABLE_LOOKUP[];

typedef enum heap_type {
	HEAP_BINARY,
	HEAP_BINOMIAL,
	HEAP_FIBONACCI,
	HEAP_LEFTIST,
	HEAP_SKEW,
	HEAP_TREAP
} heap_type_t;

typedef struct heap {
	void* heap;
	heap_type_t type;
	heap_vtable_t vtable;
} heap_t;

heap_t* heap_create(heap_type_t type);

void heap_destroy(heap_t* heap);

error_t heap_insert(heap_t* heap, request_t* value);

bool heap_is_empty(const heap_t* heap);

error_t heap_pop_max(heap_t* heap, request_t** output);

error_t heap_get_max(const heap_t* heap, request_t** output);

error_t heap_merge_new(const heap_t* a, const heap_t* b, heap_t** output);

error_t heap_merge(heap_t* a, heap_t* b);

error_t heap_meld(heap_t* in, heap_t* out);

const char* heap_error_to_string(error_t error);
