#include "fibonacci_heap.h"

#include <math.h>
#include <stdlib.h>

#include "lib/utils.h"

// =============================================================================
// VTable functions
// =============================================================================

static void* vt_create() { return fibonacci_heap_create(); }

static void vt_destroy(void* heap) {
	fibonacci_heap_destroy((fibonacci_heap_t*)heap);
}

static error_t vt_insert(void* heap, request_t* value) {
	return fibonacci_heap_insert((fibonacci_heap_t*)heap, value);
}

static bool vt_is_empty(const void* heap) {
	return fibonacci_heap_is_empty((const fibonacci_heap_t*)heap);
}

static error_t vt_pop_max(void* heap, request_t** output) {
	return fibonacci_heap_pop_max((fibonacci_heap_t*)heap, output);
}

static error_t vt_get_max(const void* heap, request_t** output) {
	return fibonacci_heap_get_max((const fibonacci_heap_t*)heap, output);
}

static error_t vt_merge_new(const void* heapA, const void* heapB,
                            void** heapOut) {
	return fibonacci_heap_merge_new((const fibonacci_heap_t*)heapA,
	                                (const fibonacci_heap_t*)heapB,
	                                (fibonacci_heap_t**)heapOut);
}

static error_t vt_merge(void* heapA, void* heapB) {
	return fibonacci_heap_merge((fibonacci_heap_t*)heapA,
	                            (fibonacci_heap_t*)heapB);
}

const heap_vtable_t FIBONACCI_HEAP_VTABLE =
    (heap_vtable_t){&vt_create,  &vt_destroy, &vt_insert,    &vt_is_empty,
                    &vt_pop_max, &vt_get_max, &vt_merge_new, &vt_merge};

// =============================================================================
// Utility functions
// =============================================================================

static fibonacci_node_t* node_create(request_t* value) {
	fibonacci_node_t* node =
	    (fibonacci_node_t*)calloc(1, sizeof(fibonacci_node_t));
	if (!node) return NULL;

	node->value = value;
	node->degree = 0;
	node->child = NULL;

	// Initialize a circular list of nodes.
	node->left = node;
	node->right = node;

	return node;
}

static void node_destroy(fibonacci_node_t* node) { free(node); }

/** Destroys all nodes in the circular list. */
static void node_destroy_all(fibonacci_node_t* node) {
	if (!node) return;

	// Iterate the circular list, destroying nodes and their children in the
	// process.
	fibonacci_node_t* cur = node;
	do {
		fibonacci_node_t* next = cur->right;

		if (next != node) {
			// if |next| == |node|, then |node| has already been freed.
			node_destroy_all(next->child);
		}
		node_destroy(cur);

		cur = next;
	} while (cur != node);
}

static void node_union(fibonacci_node_t* a, fibonacci_node_t* b) {
	if (!a || !b) return;

	fibonacci_node_t* r = a->right;
	fibonacci_node_t* l = b->left;

	// Merge two lists.
	a->right = b;
	b->left = a;

	// Link the ends.
	r->left = l;
	l->right = r;
}

static void node_link(fibonacci_node_t* child, fibonacci_node_t* parent) {
	// Remove child from the list.
	child->right->left = child->left;
	child->left->right = child->right;

	if (!parent->child) {
		parent->child = child;
		child->left = child;
		child->right = child;
	} else {
		child->left = child;
		child->right = child;
		node_union(parent->child, child);
	}

	++parent->degree;
}

static fibonacci_node_t* node_dup(const fibonacci_node_t* start) {
	if (!start) return NULL;

	// Duplicate the root node.
	fibonacci_node_t* newNode = node_create(start->value);
	if (!newNode) return NULL;

	// Iterate the node list, remembering the previous node to insert the
	// new node.
	fibonacci_node_t* prevNew = newNode;
	const fibonacci_node_t* current = start->right;

	do {
		fibonacci_node_t* newCurrent = node_create(current->value);
		if (!newCurrent) {
			node_destroy_all(newNode);
			return NULL;
		}

		// Insert |currentDup| after |prev|.
		fibonacci_node_t* prevRight = prevNew->right;
		prevNew->right = newCurrent;

		newCurrent->left = prevNew;
		newCurrent->right = prevRight;

		prevRight->left = newCurrent;

		// Clone the child, if its exists.
		if (current->child) {
			newCurrent->child = node_dup(current->child);
			if (!newCurrent->child) {
				node_destroy_all(newNode);
				return NULL;
			}
		}

		// Continue iterating.
		prevNew = newCurrent;
		current = current->right;
	} while (current != start);

	return newNode;
}

static error_t consolidate(fibonacci_heap_t* heap) {
	size_t D = (size_t)ceil(log2((double)heap->size + 1));

	fibonacci_node_t** A =
	    (fibonacci_node_t**)calloc(D, sizeof(fibonacci_node_t*));
	if (!A) return ERROR_OUT_OF_MEMORY;

	// Snapshot the root list, because I can't, for the love of God, figure out
	// why this piece of crap breaks when nodes are merged while iterating. No
	// matter how hard I try to fix it, it always finds a way to shit itself.
	vector_ptr_t rootList = vector_ptr_create();
	fibonacci_node_t* current = heap->max;

	do {
		if (!vector_ptr_push_back(&rootList, (fibonacci_node_t*)current)) {
			free(A);
			vector_ptr_destroy(&rootList);

			return ERROR_OUT_OF_MEMORY;
		}
		current = current->right;
	} while (current != heap->max);

	fibonacci_node_t *x, *y;
	unsigned d;

	for (size_t i = 0; i < vector_ptr_size(&rootList); i++) {
		x = *vector_ptr_get(&rootList, i);
		d = x->degree;

		while (A[d]) {
			y = A[d];

			if (request_priority_cmp(x->value, y->value) < 0) {
				SWAP(x, y, fibonacci_node_t*);
			}

			node_link(y, x);

			A[d] = NULL;
			++d;
		}

		A[d] = x;
	}

	for (int i = 0; i != D; i++) {
		if (A[i] && request_priority_cmp(A[i]->value, heap->max->value) > 0) {
			heap->max = A[i];
		}
	}

	free(A);
	vector_ptr_destroy(&rootList);

	return 0;
}

// =============================================================================
// Heap implementation
// =============================================================================

fibonacci_heap_t* fibonacci_heap_create() {
	return (fibonacci_heap_t*)calloc(1, sizeof(fibonacci_heap_t));
}

void fibonacci_heap_destroy(fibonacci_heap_t* heap) {
	if (heap) {
		node_destroy_all(heap->max);
		free(heap);
	}
}

error_t fibonacci_heap_insert(fibonacci_heap_t* heap, request_t* value) {
	if (!heap) return ERROR_INVALID_PARAMETER;

	fibonacci_node_t* newNode = node_create(value);
	if (!newNode) return ERROR_OUT_OF_MEMORY;

	if (heap->size == 0) {
		heap->max = newNode;
	} else {
		// Insert newNode into max.right
		fibonacci_node_t* right = heap->max->right;
		heap->max->right = newNode;

		newNode->left = heap->max;
		newNode->right = right;

		right->left = newNode;
	}

	// Update the priority if needed.
	if (request_priority_cmp(newNode->value, heap->max->value) > 0) {
		heap->max = newNode;
	}

	heap->size++;

	return 0;
}

bool fibonacci_heap_is_empty(const fibonacci_heap_t* heap) {
	return heap->size == 0;
}

error_t fibonacci_heap_pop_max(fibonacci_heap_t* heap, request_t** output) {
	if (!heap || !output) return ERROR_INVALID_PARAMETER;
	if (fibonacci_heap_is_empty(heap)) return ERROR_HEAP_EMPTY;

	fibonacci_node_t* prevMax = heap->max;
	*output = prevMax->value;

	// Merge child node list with the |max| root list.
	if (heap->max->child) {
		node_union(heap->max, heap->max->child);
	}

	// Delete |max| from the list.
	fibonacci_node_t* l = heap->max->left;
	fibonacci_node_t* r = heap->max->right;
	l->right = r;
	r->left = l;

	if (heap->max == heap->max->right) {
		// Special case for a single item in the heap.
		node_destroy(heap->max);
		heap->max = NULL;
		heap->size--;
	} else {
		heap->max = heap->max->right;
		heap->size--;
		node_destroy(prevMax);

		// Consolidate: merge nodes of the same degree.
		consolidate(heap);
	}

	return 0;
}

error_t fibonacci_heap_get_max(const fibonacci_heap_t* heap,
                               request_t** output) {
	if (!heap || !output) return ERROR_INVALID_PARAMETER;
	if (fibonacci_heap_is_empty(heap)) return ERROR_HEAP_EMPTY;

	*output = heap->max->value;

	return 0;
}

error_t fibonacci_heap_merge_new(const fibonacci_heap_t* a,
                                 const fibonacci_heap_t* b,
                                 fibonacci_heap_t** output) {
	if (!a || !b || !output) return ERROR_INVALID_PARAMETER;

	fibonacci_heap_t* aDup = fibonacci_heap_create();
	if (!aDup) return ERROR_OUT_OF_MEMORY;

	aDup->max = node_dup(a->max);
	aDup->size = a->size;
	if (!aDup->max) {
		fibonacci_heap_destroy(aDup);
		return ERROR_OUT_OF_MEMORY;
	}

	fibonacci_heap_t* bDup = fibonacci_heap_create();
	if (!bDup) {
		fibonacci_heap_destroy(aDup);
		return ERROR_OUT_OF_MEMORY;
	}

	bDup->max = node_dup(b->max);
	bDup->size = b->size;
	if (!bDup->max) {
		fibonacci_heap_destroy(aDup);
		fibonacci_heap_destroy(bDup);
		return ERROR_OUT_OF_MEMORY;
	}

	error_t error = fibonacci_heap_merge(aDup, bDup);
	if (error) {
		fibonacci_heap_destroy(aDup);
		fibonacci_heap_destroy(bDup);
		return error;
	}

	fibonacci_heap_destroy(bDup);
	*output = aDup;

	return 0;
}

error_t fibonacci_heap_merge(fibonacci_heap_t* a, fibonacci_heap_t* b) {
	if (!a || !b) return ERROR_INVALID_PARAMETER;

	if (b->size == 0) {
		return 0;
	}

	if (a->size == 0) {
		a->max = b->max;
		a->size = b->size;
	} else {
		node_union(a->max, b->max);
		a->size += b->size;
	}

	// Update the maximum if required.
	if (!a->max ||
	    (b->max && request_priority_cmp(b->max->value, a->max->value) > 0)) {
		a->max = b->max;
	}

	b->max = NULL;

	return 0;
}
