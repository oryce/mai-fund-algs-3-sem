#include "heap.h"

#include <stdlib.h>

#include "binary_heap.h"
#include "binomial_heap.h"
#include "fibonacci_heap.h"
#include "leftist_heap.h"
#include "skew_heap.h"
#include "treap.h"

const heap_vtable_t* HEAP_VTABLE_LOOKUP[] = {
    &BINARY_HEAP_VTABLE,  &BINOMIAL_HEAP_VTABLE, &FIBONACCI_HEAP_VTABLE,
    &LEFTIST_HEAP_VTABLE, &SKEW_HEAP_VTABLE,     &TREAP_VTABLE};

heap_t* heap_create(heap_type_t type) {
	heap_t* heap = (heap_t*)malloc(sizeof(heap_t));
	if (!heap) return NULL;

	heap->type = type;
	heap->vtable = *HEAP_VTABLE_LOOKUP[type];

	heap->heap = heap->vtable.create();
	if (!heap->heap) {
		free(heap);
		return NULL;
	}

	return heap;
}

void heap_destroy(heap_t* heap) {
	if (heap) {
		heap->vtable.destroy(heap->heap);
		free(heap);
	}
}

error_t heap_insert(heap_t* heap, request_t* value) {
	if (!heap) return ERROR_INVALID_PARAMETER;
	return heap->vtable.insert(heap->heap, value);
}

bool heap_is_empty(const heap_t* heap) {
	if (!heap) return false;
	return heap->vtable.is_empty(heap->heap);
}

error_t heap_pop_max(heap_t* heap, request_t** output) {
	if (!heap) return ERROR_INVALID_PARAMETER;
	return heap->vtable.pop_max(heap->heap, output);
}

error_t heap_get_max(const heap_t* heap, request_t** output) {
	if (!heap) return ERROR_INVALID_PARAMETER;
	return heap->vtable.get_max(heap->heap, output);
}

error_t heap_merge_new(const heap_t* a, const heap_t* b, heap_t** output) {
	if (!a || !b || !output) return ERROR_INVALID_PARAMETER;
	if (a->type != b->type) return ERROR_HEAP_INCOMPATIBLE;

	*output = (heap_t*)malloc(sizeof(heap_t));
	if (!(*output)) return ERROR_OUT_OF_MEMORY;

	(*output)->heap = NULL;
	(*output)->type = a->type;
	(*output)->vtable = a->vtable;

	return a->vtable.merge_new(a->heap, b->heap, &(*output)->heap);
}

error_t heap_merge(heap_t* a, heap_t* b) {
	if (!a || !b) return ERROR_INVALID_PARAMETER;
	if (a->type != b->type) return ERROR_HEAP_INCOMPATIBLE;

	return a->vtable.merge(a->heap, b->heap);
}

error_t heap_meld(heap_t* in, heap_t* out) {
	if (!in || !out) return ERROR_INVALID_PARAMETER;

	while (!heap_is_empty(in)) {
		request_t* request;

		error_t error = heap_pop_max(in, &request);
		if (error) return error;
		error = heap_insert(out, request);
		if (error) return error;
	}

	return 0;
}

const char* heap_error_to_string(error_t error) {
	switch (error) {
		case ERROR_HEAP_EMPTY:
			return "The heap is empty";
		case ERROR_HEAP_INCOMPATIBLE:
			return "This heap cannot be merged with a heap of a different "
			       "kind.";
		default:
			return NULL;
	}
}
