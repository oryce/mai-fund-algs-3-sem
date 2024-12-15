#include "binomial_heap.h"

#include <stdlib.h>

// =============================================================================
// VTable functions
// =============================================================================

static void* vt_create() { return binomial_heap_create(); }

static void vt_destroy(void* heap) {
	binomial_heap_destroy((binomial_heap_t*)heap);
}

static error_t vt_insert(void* heap, request_t* value) {
	return binomial_heap_insert((binomial_heap_t*)heap, value);
}

static bool vt_is_empty(const void* heap) {
	return binomial_heap_is_empty((const binomial_heap_t*)heap);
}

static error_t vt_pop_max(void* heap, request_t** output) {
	return binomial_heap_pop_max((binomial_heap_t*)heap, output);
}

static error_t vt_get_max(const void* heap, request_t** output) {
	return binomial_heap_get_max((const binomial_heap_t*)heap, output);
}

static error_t vt_merge_new(const void* heapA, const void* heapB,
                            void** heapOut) {
	return binomial_heap_merge_new((const binomial_heap_t*)heapA,
	                               (const binomial_heap_t*)heapB,
	                               (binomial_heap_t**)heapOut);
}

static error_t vt_merge(void* heapA, void* heapB) {
	return binomial_heap_merge((binomial_heap_t*)heapA,
	                           (binomial_heap_t*)heapB);
}

const heap_vtable_t BINOMIAL_HEAP_VTABLE =
    (heap_vtable_t){&vt_create,  &vt_destroy, &vt_insert,    &vt_is_empty,
                    &vt_pop_max, &vt_get_max, &vt_merge_new, &vt_merge};

// =============================================================================
// Utility functions
// =============================================================================

static binomial_tree_t* node_create(request_t* value) {
	binomial_tree_t* node = (binomial_tree_t*)malloc(sizeof(binomial_tree_t));
	if (!node) return NULL;

	node->value = value;
	node->child = NULL;
	node->sibling = NULL;
	node->degree = 0;

	return node;
}

static void node_destroy(binomial_tree_t* node) {
	if (!node) return;

	if (node->child) node_destroy(node->child);
	if (node->sibling) node_destroy(node->sibling);

	free(node);
}

static binomial_tree_t* node_merge(binomial_tree_t* a, binomial_tree_t* b) {
	if (!a) return b;
	if (!b) return a;

	binomial_tree_t* head = NULL;

	// Find a node with the smallest degree.
	if (a->degree < b->degree) {
		head = a;
		a = a->sibling;
	} else {
		head = b;
		b = b->sibling;
	}

	binomial_tree_t* cur = head;

	while (a && b) {
		if (a->degree < b->degree) {
			cur->sibling = a;
			a = a->sibling;
		} else {
			cur->sibling = b;
			b = b->sibling;
		}
		cur = cur->sibling;
	}

	if (a) cur->sibling = a;
	if (b) cur->sibling = b;

	return head;
}

static void node_attach(binomial_tree_t* node, binomial_tree_t* child) {
	if (!node || !child) return;

	child->sibling = node->child;

	node->child = child;
	node->degree++;
}

static binomial_tree_t* node_dup(const binomial_tree_t* node) {
	if (!node) return NULL;

	binomial_tree_t* dup = node_create(node->value);
	if (!dup) return NULL;

	if (node->child) {
		dup->child = node_dup(node->child);
		if (!dup->child) {
			node_destroy(dup);
			return NULL;
		}
	}
	if (node->sibling) {
		dup->sibling = node_dup(node->sibling);
		if (!dup->sibling) {
			node_destroy(dup);
			return NULL;
		}
	}

	return dup;
}

// =============================================================================
// Heap implementation
// =============================================================================

binomial_heap_t* binomial_heap_create() {
	return (binomial_heap_t*)calloc(1, sizeof(binomial_heap_t));
}

void binomial_heap_destroy(binomial_heap_t* heap) {
	if (heap) {
		node_destroy(heap->root);
		free(heap);
	}
}

error_t binomial_heap_insert(binomial_heap_t* heap, request_t* value) {
	if (!heap) return ERROR_INVALID_PARAMETER;

	// Create a heap only with the inserted element.
	binomial_heap_t* elementHeap = binomial_heap_create();
	if (!elementHeap) return ERROR_OUT_OF_MEMORY;

	elementHeap->root = node_create(value);
	if (!elementHeap->root) {
		binomial_heap_destroy(elementHeap);
		return ERROR_OUT_OF_MEMORY;
	}

	// Merge this heap with a heap of the added element.
	error_t error = binomial_heap_merge(heap, elementHeap);

	binomial_heap_destroy(elementHeap);

	return error;
}

bool binomial_heap_is_empty(const binomial_heap_t* heap) {
	return heap->root == NULL;
}

error_t binomial_heap_pop_max(binomial_heap_t* heap, request_t** output) {
	if (!heap || !output) return ERROR_INVALID_PARAMETER;
	if (binomial_heap_is_empty(heap)) return ERROR_HEAP_EMPTY;

	// Find the largest root in the heap.
	binomial_tree_t** largestRoot = &heap->root;

	for (binomial_tree_t** node = &(*largestRoot)->sibling; *node;
	     node = &(*node)->sibling) {
		if (request_priority_cmp((*node)->value, (*largestRoot)->value) > 0) {
			largestRoot = node;
		}
	}

	*output = (*largestRoot)->value;

	// Copy its members for later use, because we'll `free` it now.
	binomial_tree_t* child = (*largestRoot)->child;
	binomial_tree_t* sibling = (*largestRoot)->sibling;

	// Remove the largest root by replacing it with its sibling.
	free(*largestRoot);
	*largestRoot = sibling;

	if (child) {
		// Reverse the list of children, because the merge operation expects
		// heaps with roots in ascending order of degree.
		binomial_tree_t *prev = NULL, *next;

		for (binomial_tree_t* node = child; node;) {
			next = node->sibling;
			node->sibling = prev;

			prev = node;
			node = next;
		}

		// Merge the heap with a heap of children.
		binomial_heap_t* newHeap = binomial_heap_create();
		if (!newHeap) return ERROR_OUT_OF_MEMORY;

		newHeap->root = prev;

		binomial_heap_merge(heap, newHeap);
		binomial_heap_destroy(newHeap);
	}

	return 0;
}

error_t binomial_heap_get_max(const binomial_heap_t* heap, request_t** output) {
	if (!heap || !output) return ERROR_INVALID_PARAMETER;

	binomial_tree_t* largestRoot = heap->root;

	for (binomial_tree_t* node = largestRoot->sibling; node;
	     node = node->sibling) {
		if (request_priority_cmp(node->value, largestRoot->value) > 0) {
			largestRoot = node;
		}
	}

	*output = largestRoot->value;
	return 0;
}

error_t binomial_heap_merge_new(const binomial_heap_t* a,
                                const binomial_heap_t* b,
                                binomial_heap_t** output) {
	if (!a || !b || !output) return ERROR_INVALID_PARAMETER;

	binomial_heap_t* aDup = binomial_heap_create();
	if (!aDup) return ERROR_OUT_OF_MEMORY;

	aDup->root = node_dup(a->root);
	if (!aDup->root) {
		binomial_heap_destroy(aDup);
		return ERROR_OUT_OF_MEMORY;
	}

	binomial_heap_t* bDup = binomial_heap_create();
	if (!bDup) {
		binomial_heap_destroy(aDup);
		return ERROR_OUT_OF_MEMORY;
	}

	bDup->root = node_dup(b->root);
	if (!bDup->root) {
		binomial_heap_destroy(aDup);
		binomial_heap_destroy(bDup);
		return ERROR_OUT_OF_MEMORY;
	}

	error_t error = binomial_heap_merge(aDup, bDup);
	if (error) {
		binomial_heap_destroy(aDup);
		binomial_heap_destroy(bDup);
		return error;
	}

	binomial_heap_destroy(bDup);
	*output = aDup;

	return 0;
}

error_t binomial_heap_merge(binomial_heap_t* a, binomial_heap_t* b) {
	if (!a || !b) return ERROR_INVALID_PARAMETER;

	// (1) Merge forests
	a->root = node_merge(a->root, b->root);
	b->root = NULL;  // NB: caller must call b_h_destroy(b)

	if (!a->root) return 0;

	// (2) Consolidate: merge trees of the same degree
	binomial_tree_t* prev = NULL;
	binomial_tree_t* cur = a->root;
	binomial_tree_t* next = cur->sibling;

	while (next) {
		bool merge = cur->degree == next->degree &&
		             (!next->sibling || next->degree != next->sibling->degree);

		if (merge) {
			if (request_priority_cmp(cur->value, next->value) >= 0) {
				cur->sibling = next->sibling;
				node_attach(cur, next);
			} else /* node < next */ {
				if (!prev) {
					a->root = next;
				} else {
					prev->sibling = next;
				}

				node_attach(next, cur);
				cur = next;
			}
		} else {
			prev = cur;
			cur = next;
		}

		next = cur->sibling;
	}

	return 0;
}
