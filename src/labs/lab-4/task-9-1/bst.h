#pragma once

#include "department.h"
#include "storage.h"

extern const storage_vtable_t BST_VTABLE;

typedef struct bst {
	const char* key;
	department_t* value;
	struct bst* left;
	struct bst* right;
} bst_t;

bst_t* bst_create();

void bst_destroy(bst_t* bst);

department_t* bst_get(const bst_t* bst, const char* key);

error_t bst_put(bst_t* bst, const char* key, department_t* value);

size_t bst_size(const bst_t* bst);
