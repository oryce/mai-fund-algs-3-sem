#include "treap.h"

#include <stdlib.h>
#include <time.h>

typedef struct treap_pair {
	treap_node_t* t1;
	treap_node_t* t2;
} treap_pair_t;

// =============================================================================
// VTable functions
// =============================================================================

static void* vt_create() { return treap_create(); }

static void vt_destroy(void* heap) { treap_destroy((treap_t*)heap); }

static error_t vt_insert(void* heap, request_t* value) {
	return treap_insert((treap_t*)heap, value);
}

static bool vt_is_empty(const void* heap) {
	return treap_is_empty((const treap_t*)heap);
}

static error_t vt_pop_max(void* heap, request_t** output) {
	return treap_pop_max((treap_t*)heap, output);
}

static error_t vt_get_max(const void* heap, request_t** output) {
	return treap_get_max((const treap_t*)heap, output);
}

static error_t vt_merge_new(const void* heapA, const void* heapB,
                            void** heapOut) {
	return treap_merge_new((const treap_t*)heapA, (const treap_t*)heapB,
	                       (treap_t**)heapOut);
}

static error_t vt_merge(void* heapA, void* heapB) {
	return treap_merge((treap_t*)heapA, (treap_t*)heapB);
}

const heap_vtable_t TREAP_VTABLE =
    (heap_vtable_t){&vt_create,  &vt_destroy, &vt_insert,    &vt_is_empty,
                    &vt_pop_max, &vt_get_max, &vt_merge_new, &vt_merge};

// =============================================================================
// Utility functions
// =============================================================================

static treap_node_t* node_create(request_t* value) {
	treap_node_t* node = (treap_node_t*)malloc(sizeof(treap_node_t));
	if (!node) return NULL;

	node->value = value;
	node->x = rand();
	node->left = NULL;
	node->right = NULL;

	return node;
}

static void node_destroy(treap_node_t* node) {
	if (!node) return;

	if (node->left) node_destroy(node->left);
	if (node->right) node_destroy(node->right);

	free(node);
}

static treap_pair_t node_split(treap_node_t* t, int k) {
	if (!t) {
		return (treap_pair_t){NULL, NULL};
	}

	if (k > t->x) {
		treap_pair_t pair = node_split(t->right, k);
		t->right = pair.t1;
		return (treap_pair_t){t, pair.t2};
	} else {
		treap_pair_t pair = node_split(t->left, k);
		t->left = pair.t2;
		return (treap_pair_t){pair.t1, t};
	}
}

treap_node_t* node_merge(treap_node_t* t1, treap_node_t* t2) {
	if (!t2) return t1;
	if (!t1) return t2;

	if (request_priority_cmp(t1->value, t2->value) > 0) {
		t1->right = node_merge(t1->right, t2);
		return t1;
	} else {
		t2->left = node_merge(t1, t2->left);
		return t2;
	}
}

static treap_node_t* node_dup(const treap_node_t* node) {
	if (!node) return NULL;

	treap_node_t* dup = node_create(node->value);
	if (!dup) return NULL;

	// Copy the X value
	dup->x = node->x;

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

treap_t* treap_create() {
	// Seed the RNG for X values of Treap nodes
	srand(time(NULL));

	return (treap_t*)calloc(1, sizeof(treap_t));
}

void treap_destroy(treap_t* treap) {
	if (treap) {
		node_destroy(treap->root);
		free(treap);
	}
}

error_t treap_insert(treap_t* heap, request_t* value) {
	if (!heap) return ERROR_INVALID_PARAMETER;

	treap_node_t* node = node_create(value);
	if (!node) return ERROR_OUT_OF_MEMORY;

	// Split the tree by key |x|.
	treap_pair_t pair = node_split(heap->root, node->x);

	// Merge the adding node with the left side of the split.
	treap_node_t* t1 = node_merge(pair.t1, node);
	// Merge the node with the tree with the right side of the split.
	heap->root = node_merge(t1, pair.t2);

	return 0;
}

bool treap_is_empty(const treap_t* heap) { return heap->root == NULL; }

error_t treap_pop_max(treap_t* heap, request_t** output) {
	if (!heap || !output) return ERROR_INVALID_PARAMETER;
	if (treap_is_empty(heap)) return ERROR_HEAP_EMPTY;

	treap_node_t* l = heap->root->left;
	treap_node_t* r = heap->root->right;

	*output = heap->root->value;
	free(heap->root);

	heap->root = node_merge(l, r);
	return 0;
}

error_t treap_get_max(const treap_t* heap, request_t** output) {
	if (!heap || !output) return ERROR_INVALID_PARAMETER;
	if (treap_is_empty(heap)) return ERROR_HEAP_EMPTY;

	*output = heap->root->value;
	return 0;
}

error_t treap_merge_new(const treap_t* a, const treap_t* b, treap_t** output) {
	if (!a || !b || !output) return ERROR_INVALID_PARAMETER;

	treap_node_t* aDup = node_dup(a->root);
	if (a->root && !aDup) return ERROR_OUT_OF_MEMORY;

	treap_node_t* bDup = node_dup(b->root);
	if (b->root && !bDup) {
		node_destroy(aDup);
		return ERROR_OUT_OF_MEMORY;
	}

	*output = treap_create();
	if (!(*output)) {
		node_destroy(aDup);
		node_destroy(bDup);
		return ERROR_OUT_OF_MEMORY;
	}

	(*output)->root = node_merge(aDup, bDup);

	return 0;
}

error_t treap_merge(treap_t* a, treap_t* b) {
	if (!a || !b) return ERROR_INVALID_PARAMETER;

	a->root = node_merge(a->root, b->root);
	b->root = NULL;

	return 0;
}
