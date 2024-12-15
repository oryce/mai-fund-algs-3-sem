#include "binary_heap.h"

#include <stdlib.h>

#include "lib/utils.h"

static const int MIN_CAPACITY = 4;

// =============================================================================
// VTable functions
// =============================================================================

static void* vt_create() { return binary_heap_create(); }

static void vt_destroy(void* heap) {
	binary_heap_destroy((binary_heap_t*)heap);
}

static error_t vt_insert(void* heap, request_t* value) {
	return binary_heap_insert((binary_heap_t*)heap, value);
}

static bool vt_is_empty(const void* heap) {
	return binary_heap_is_empty((const binary_heap_t*)heap);
}

static error_t vt_pop_max(void* heap, request_t** output) {
	return binary_heap_pop_max((binary_heap_t*)heap, output);
}

static error_t vt_get_max(const void* heap, request_t** output) {
	return binary_heap_get_max((const binary_heap_t*)heap, output);
}

static error_t vt_merge_new(const void* heapA, const void* heapB,
                            void** heapOut) {
	return binary_heap_merge_new((const binary_heap_t*)heapA,
	                             (const binary_heap_t*)heapB,
	                             (binary_heap_t**)heapOut);
}

static error_t vt_merge(void* heapA, void* heapB) {
	return binary_heap_merge((binary_heap_t*)heapA, (binary_heap_t*)heapB);
}

const heap_vtable_t BINARY_HEAP_VTABLE =
    (heap_vtable_t){&vt_create,  &vt_destroy, &vt_insert,    &vt_is_empty,
                    &vt_pop_max, &vt_get_max, &vt_merge_new, &vt_merge};

// =============================================================================
// Utility functions
// =============================================================================

static int compare(const binary_heap_t* heap, size_t i, size_t j) {
	if (!heap || i >= heap->size || j >= heap->size) return 0;

	return request_priority_cmp(heap->buffer[i].value, heap->buffer[j].value);
}

static void sift_up(binary_heap_t* heap, size_t i) {
	if (!heap || i >= heap->size) return;

	// "heap[i] > heap[parent(i)]"
	while (i && compare(heap, i, (i - 1) / 2) > 0) {
		SWAP(heap->buffer[i], heap->buffer[(i - 1) / 2], bheap_node_t);
		i = (i - 1) / 2;
	}
}

static void sift_down(binary_heap_t* heap, size_t i) {
	if (!heap || i >= heap->size) return;

	size_t j;

	while (2 * i + 1 < heap->size) {
		size_t left = 2 * i + 1;
		size_t right = 2 * i + 2;

		j = left;

		if (right < heap->size && compare(heap, right, left) > 0) {
			j = right;
		}
		if (compare(heap, i, j) >= 0) {
			break;
		}

		SWAP(heap->buffer[i], heap->buffer[j], bheap_node_t);
		i = j;
	}
}

// =============================================================================
// Heap implementation
// =============================================================================

binary_heap_t* binary_heap_create() {
	binary_heap_t* heap = (binary_heap_t*)malloc(sizeof(binary_heap_t));
	if (!heap) return NULL;

	heap->buffer = NULL;
	heap->size = 0;
	heap->capacity = MIN_CAPACITY;

	return heap;
}

void binary_heap_destroy(binary_heap_t* heap) {
	if (heap) {
		free(heap->buffer);
		free(heap);
	}
}

error_t binary_heap_insert(binary_heap_t* heap, request_t* value) {
	if (!heap) return ERROR_INVALID_PARAMETER;

	if (!heap->buffer) {
		heap->buffer =
		    (bheap_node_t*)malloc(heap->capacity * sizeof(bheap_node_t));
		if (!heap->buffer) return ERROR_OUT_OF_MEMORY;
	} else if (heap->size == heap->capacity) {
		heap->capacity *= 2;

		bheap_node_t* newBuffer = (bheap_node_t*)realloc(
		    heap->buffer, heap->capacity * sizeof(bheap_node_t));
		if (!newBuffer) return ERROR_OUT_OF_MEMORY;

		heap->buffer = newBuffer;
	}

	heap->buffer[heap->size++] = (bheap_node_t){value};
	sift_up(heap, heap->size - 1);

	return 0;
}

bool binary_heap_is_empty(const binary_heap_t* heap) { return heap->size == 0; }

error_t binary_heap_pop_max(binary_heap_t* heap, request_t** output) {
	if (!heap) return ERROR_INVALID_PARAMETER;
	if (!heap->size) return ERROR_HEAP_EMPTY;

	bheap_node_t max = heap->buffer[0];

	heap->buffer[0] = heap->buffer[--heap->size];
	sift_down(heap, 0);

	if (heap->capacity > MIN_CAPACITY && heap->size < heap->capacity / 2) {
		bheap_node_t* newBuffer = (bheap_node_t*)realloc(
		    heap->buffer, heap->capacity / 2 * sizeof(bheap_node_t));
		if (!newBuffer) return ERROR_OUT_OF_MEMORY;

		heap->buffer = newBuffer;
		heap->capacity /= 2;
	}

	*output = max.value;
	return 0;
}

error_t binary_heap_get_max(const binary_heap_t* heap, request_t** output) {
	if (!heap || !output) return ERROR_INVALID_PARAMETER;
	if (!heap->size) return ERROR_HEAP_EMPTY;

	*output = heap->buffer[0].value;
	return 0;
}

error_t binary_heap_merge_new(const binary_heap_t* a, const binary_heap_t* b,
                              binary_heap_t** out) {
	if (!a || !b || !out) return ERROR_INVALID_PARAMETER;

	*out = binary_heap_create();
	if (!(*out)) return ERROR_OUT_OF_MEMORY;

	// No-op is both heaps are empty.
	if (binary_heap_is_empty(a) && binary_heap_is_empty(b)) {
		return 0;
	}

	// Initialize a new heap that fits both heaps.
	(*out)->capacity = a->size + b->size;
	(*out)->buffer =
	    (bheap_node_t*)malloc((*out)->capacity * sizeof(bheap_node_t));
	if (!(*out)->buffer) {
		binary_heap_destroy(*out);
		return ERROR_OUT_OF_MEMORY;
	}

	// Copy items from A and B to output.
	memcpy((*out)->buffer, a->buffer, a->size * sizeof(bheap_node_t));
	memcpy((*out)->buffer + a->size, b->buffer, b->size * sizeof(bheap_node_t));

	(*out)->size = a->size + b->size;

	for (size_t i = a->size - 1; i--;) {
		sift_down(*out, i);
	}

	return 0;
}

error_t binary_heap_merge(binary_heap_t* a, binary_heap_t* b) {
	if (!a || !b) return ERROR_INVALID_PARAMETER;

	// Merging heap is empty; no action.
	if (!b->size) return 0;

	// Empty newly-created heap.
	if (!a->buffer && a->capacity) {
		a->buffer = (bheap_node_t*)malloc(b->size * sizeof(bheap_node_t));
		if (!a->buffer) return ERROR_OUT_OF_MEMORY;

		a->capacity = b->size;
	}
	// A cannot fit heap B.
	else if (a->capacity < a->size + b->size) {
		bheap_node_t* newBuffer = (bheap_node_t*)realloc(
		    a->buffer, (a->size + b->size) * sizeof(bheap_node_t));
		if (!newBuffer) return ERROR_OUT_OF_MEMORY;

		a->buffer = newBuffer;
		a->capacity = a->size + b->size;
	}

	// Copy elements from B to the end of A
	memcpy(a->buffer + a->size, b->buffer, b->size * sizeof(bheap_node_t));
	a->size += b->size;

	for (size_t i = a->size - 1; i--;) {
		sift_down(a, i);
	}

	free(b->buffer);
	b->buffer = NULL;

	return 0;
}
