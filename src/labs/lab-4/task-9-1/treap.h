#pragma once

#include "heap.h"
#include "lib/error.h"

extern const heap_vtable_t TREAP_VTABLE;

typedef struct treap_node {
	request_t* value;

	/** Key value. */
	int x;

	struct treap_node* left;
	struct treap_node* right;
} treap_node_t;

typedef struct treap {
	treap_node_t* root;
} treap_t;

treap_t* treap_create();

void treap_destroy(treap_t* treap);

error_t treap_insert(treap_t* heap, request_t* value);

bool treap_is_empty(const treap_t* heap);

error_t treap_pop_max(treap_t* heap, request_t** output);

error_t treap_get_max(const treap_t* heap, request_t** output);

error_t treap_merge_new(const treap_t* a, const treap_t* b, treap_t** output);

error_t treap_merge(treap_t* a, treap_t* b);
