#include "leftist_heap.h"

#include <stdlib.h>

#include "lib/utils.h"

// =============================================================================
// VTable functions
// =============================================================================

static void* vt_create() { return leftist_heap_create(); }

static void vt_destroy(void* heap) {
	leftist_heap_destroy((leftist_heap_t*)heap);
}

static error_t vt_insert(void* heap, request_t* value) {
	return leftist_heap_insert((leftist_heap_t*)heap, value);
}

static bool vt_is_empty(const void* heap) {
	return leftist_heap_is_empty((const leftist_heap_t*)heap);
}

static error_t vt_pop_max(void* heap, request_t** output) {
	return leftist_heap_pop_max((leftist_heap_t*)heap, output);
}

static error_t vt_get_max(const void* heap, request_t** output) {
	return leftist_heap_get_max((const leftist_heap_t*)heap, output);
}

static error_t vt_merge_new(const void* heapA, const void* heapB,
                            void** heapOut) {
	return leftist_heap_merge_new((const leftist_heap_t*)heapA,
	                              (const leftist_heap_t*)heapB,
	                              (leftist_heap_t**)heapOut);
}

static error_t vt_merge(void* heapA, void* heapB) {
	return leftist_heap_merge((leftist_heap_t*)heapA, (leftist_heap_t*)heapB);
}

const heap_vtable_t LEFTIST_HEAP_VTABLE =
    (heap_vtable_t){&vt_create,  &vt_destroy, &vt_insert,    &vt_is_empty,
                    &vt_pop_max, &vt_get_max, &vt_merge_new, &vt_merge};

// =============================================================================
// Utility functions
// =============================================================================

static leftist_node_t* node_create(request_t* value) {
	leftist_node_t* node = (leftist_node_t*)malloc(sizeof(leftist_node_t));
	if (!node) return NULL;

	node->value = value;
	node->left = NULL;
	node->right = NULL;
	node->npl = 0;

	return node;
}

static void node_destroy(leftist_node_t* node) {
	if (!node) return;

	if (node->left) node_destroy(node->left);
	if (node->right) node_destroy(node->right);

	free(node);
}

static leftist_node_t* node_merge(leftist_node_t* a, leftist_node_t* b) {
	if (!a) return b;
	if (!b) return a;

	if (request_priority_cmp(a->value, b->value) < 0) {
		SWAP(a, b, leftist_node_t*);
	}

	// Воспользуемся тем, что куча левосторонняя. Правая ветка — самая короткая
	// и не длиннее логарифма. Пойдем направо и сольем правое поддерево с у.
	a->right = node_merge(a->right, b);

	// Могло возникнуть нарушение левостороннести кучи
	if (!a->left || a->right->npl > a->left->npl) {
		SWAP(a->left, a->right, leftist_node_t*);
	}

	// пересчитаем расстояние до ближайшей свободной позиции
	if (a->right) {
		a->npl = a->right->npl + 1;
	} else {
		a->npl = 0;
	}

	return a;
}

static leftist_node_t* node_dup(const leftist_node_t* node) {
	if (!node) return NULL;

	leftist_node_t* dup = node_create(node->value);
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

leftist_heap_t* leftist_heap_create() {
	return (leftist_heap_t*)calloc(1, sizeof(leftist_heap_t));
}

void leftist_heap_destroy(leftist_heap_t* heap) {
	if (!heap) return;

	node_destroy(heap->root);

	free(heap);
}

error_t leftist_heap_insert(leftist_heap_t* heap, request_t* value) {
	if (!heap) return ERROR_INVALID_PARAMETER;

	leftist_node_t* node = node_create(value);
	if (!node) return ERROR_OUT_OF_MEMORY;

	heap->root = node_merge(heap->root, node);
	return 0;
}

bool leftist_heap_is_empty(const leftist_heap_t* heap) {
	return heap->root == NULL;
}

error_t leftist_heap_pop_max(leftist_heap_t* heap, request_t** output) {
	if (!heap || !output) return ERROR_INVALID_PARAMETER;
	if (leftist_heap_is_empty(heap)) return ERROR_HEAP_EMPTY;

	leftist_node_t* root = heap->root;

	*output = root->value;
	heap->root = node_merge(root->left, root->right);

	free(root);
	return 0;
}

error_t leftist_heap_get_max(const leftist_heap_t* heap, request_t** output) {
	if (!heap || !output) return ERROR_INVALID_PARAMETER;
	if (leftist_heap_is_empty(heap)) return ERROR_HEAP_EMPTY;

	*output = heap->root->value;
	return 0;
}

error_t leftist_heap_merge_new(const leftist_heap_t* a, const leftist_heap_t* b,
                               leftist_heap_t** output) {
	if (!a || !b || !output) return ERROR_INVALID_PARAMETER;

	leftist_node_t* aDup = node_dup(a->root);
	if (a->root && !aDup) return ERROR_OUT_OF_MEMORY;

	leftist_node_t* bDup = node_dup(b->root);
	if (b->root && !bDup) {
		node_destroy(aDup);
		return ERROR_OUT_OF_MEMORY;
	}

	*output = leftist_heap_create();
	if (!(*output)) {
		node_destroy(aDup);
		node_destroy(bDup);
		return ERROR_OUT_OF_MEMORY;
	}

	(*output)->root = node_merge(aDup, bDup);

	return 0;
}

error_t leftist_heap_merge(leftist_heap_t* a, leftist_heap_t* b) {
	if (!a || !b) return ERROR_INVALID_PARAMETER;

	a->root = node_merge(a->root, b->root);
	b->root = NULL;

	return 0;
}
