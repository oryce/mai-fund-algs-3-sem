#include "skew_heap.h"

#include <stdlib.h>

// =============================================================================
// VTable functions
// =============================================================================

static void* vt_create() { return skew_heap_create(); }

static void vt_destroy(void* heap) { skew_heap_destroy((skew_heap_t*)heap); }

static error_t vt_insert(void* heap, request_t* value) {
	return skew_heap_insert((skew_heap_t*)heap, value);
}

static bool vt_is_empty(const void* heap) {
	return skew_heap_is_empty((const skew_heap_t*)heap);
}

static error_t vt_pop_max(void* heap, request_t** output) {
	return skew_heap_pop_max((skew_heap_t*)heap, output);
}

static error_t vt_get_max(const void* heap, request_t** output) {
	return skew_heap_get_max((const skew_heap_t*)heap, output);
}

static error_t vt_merge_new(const void* heapA, const void* heapB,
                            void** heapOut) {
	return skew_heap_merge_new((const skew_heap_t*)heapA,
	                           (const skew_heap_t*)heapB,
	                           (skew_heap_t**)heapOut);
}

static error_t vt_merge(void* heapA, void* heapB) {
	return skew_heap_merge((skew_heap_t*)heapA, (skew_heap_t*)heapB);
}

const heap_vtable_t SKEW_HEAP_VTABLE =
    (heap_vtable_t){&vt_create,  &vt_destroy, &vt_insert,    &vt_is_empty,
                    &vt_pop_max, &vt_get_max, &vt_merge_new, &vt_merge};

// =============================================================================
// Utility functions
// =============================================================================

static skew_node_t* node_create(request_t* value) {
	skew_node_t* node = (skew_node_t*)malloc(sizeof(skew_node_t));
	if (!node) return NULL;

	node->value = value;
	node->left = NULL;
	node->right = NULL;

	return node;
}

static void node_destroy(skew_node_t* node) {
	if (!node) return;

	if (node->left) node_destroy(node->left);
	if (node->right) node_destroy(node->right);

	free(node);
}

static skew_node_t* node_merge(skew_node_t* a, skew_node_t* b) {
	if (!a) return b;
	if (!b) return a;

	if (request_priority_cmp(a->value, b->value) >= 0) {
		skew_node_t* temp = a->right;
		a->right = a->left;
		a->left = node_merge(b, temp);
		return a;
	} else {
		return node_merge(b, a);
	}
}

static skew_node_t* node_dup(const skew_node_t* node) {
	if (!node) return NULL;

	skew_node_t* dup = node_create(node->value);
	if (!dup) return NULL;

	if (node->left) {
		dup->left = node_dup(node->left);
		if (!dup->left) {
			node_destroy(dup);
			return NULL;
		}
	}
	if (node->right) {
		dup->right = node_dup(node->right);
		if (!dup->right) {
			node_destroy(dup);
			return NULL;
		}
	}

	return dup;
}

// =============================================================================
// Heap implementation
// =============================================================================

skew_heap_t* skew_heap_create() {
	return (skew_heap_t*)calloc(1, sizeof(skew_heap_t));
}

void skew_heap_destroy(skew_heap_t* heap) {
	if (!heap) return;

	node_destroy(heap->root);

	free(heap);
}

error_t skew_heap_insert(skew_heap_t* heap, request_t* value) {
	if (!heap) return ERROR_INVALID_PARAMETER;

	skew_node_t* node = node_create(value);
	if (!node) return ERROR_OUT_OF_MEMORY;

	heap->root = node_merge(heap->root, node);
	return 0;
}

bool skew_heap_is_empty(const skew_heap_t* heap) { return heap->root == NULL; }

error_t skew_heap_pop_max(skew_heap_t* heap, request_t** output) {
	if (!heap || !output) return ERROR_INVALID_PARAMETER;
	if (skew_heap_is_empty(heap)) return ERROR_HEAP_EMPTY;

	skew_node_t* root = heap->root;

	*output = root->value;
	heap->root = node_merge(root->left, root->right);

	free(root);
	return 0;
}

error_t skew_heap_get_max(const skew_heap_t* heap, request_t** output) {
	if (!heap || !output) return ERROR_INVALID_PARAMETER;
	if (skew_heap_is_empty(heap)) return ERROR_HEAP_EMPTY;

	*output = heap->root->value;
	return 0;
}

error_t skew_heap_merge_new(const skew_heap_t* a, const skew_heap_t* b,
                            skew_heap_t** output) {
	if (!a || !b || !output) return ERROR_INVALID_PARAMETER;

	skew_node_t* aDup = node_dup(a->root);
	if (a->root && !aDup) return ERROR_OUT_OF_MEMORY;

	skew_node_t* bDup = node_dup(b->root);
	if (b->root && !bDup) {
		node_destroy(aDup);
		return ERROR_OUT_OF_MEMORY;
	}

	*output = skew_heap_create();
	if (!(*output)) {
		node_destroy(aDup);
		node_destroy(bDup);
		return ERROR_OUT_OF_MEMORY;
	}

	(*output)->root = node_merge(aDup, bDup);

	return 0;
}

error_t skew_heap_merge(skew_heap_t* a, skew_heap_t* b) {
	if (!a || !b) return ERROR_INVALID_PARAMETER;

	a->root = node_merge(a->root, b->root);
	b->root = NULL;

	return 0;
}
