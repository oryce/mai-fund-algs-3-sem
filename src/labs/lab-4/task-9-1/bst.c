#include "bst.h"

#include <stdlib.h>

// =============================================================================
// VTable functions
// =============================================================================

static void* vt_create(void) { return (void*)bst_create(); }

static void vt_destroy(void* storage) { bst_destroy((bst_t*)storage); }

static department_t* vt_get(const void* storage, const char* key) {
	return bst_get((bst_t*)storage, key);
}

static error_t vt_put(void* storage, const char* key, department_t* value) {
	return bst_put((bst_t*)storage, key, value);
}

const storage_vtable_t BST_VTABLE = {&vt_create, &vt_destroy, &vt_get, &vt_put};

// =============================================================================
// Storage implementation
// =============================================================================

bst_t* bst_create() { return (bst_t*)calloc(1, sizeof(bst_t)); }

void bst_destroy(bst_t* bst) {
	if (!bst) return;

	if (bst->left) bst_destroy(bst->left);
	if (bst->right) bst_destroy(bst->right);

	free(bst->key);
	free(bst);
}

department_t* bst_get(const bst_t* bst, const char* key) {
	if (!bst || !key) return NULL;

	int order = strcmp(key, bst->key);

	if (order == 0) {
		return bst->value;
	} else if (order < 0) {
		return bst_get(bst->left, key);
	} else {
		return bst_get(bst->right, key);
	}
}

static error_t put_helper(bst_t** bst, const char* key, department_t* value) {
	if (!bst || !key) return ERROR_INVALID_PARAMETER;

	if (*bst == NULL || (*bst)->key == NULL /* uninitialized root node */) {
		if (*bst == NULL) {
			*bst = bst_create();
			if (*bst == NULL) return ERROR_OUT_OF_MEMORY;
		}

		(*bst)->key = strdup(key);
		(*bst)->value = value;

		if ((*bst)->key == NULL) {
			bst_destroy(*bst);
			return ERROR_OUT_OF_MEMORY;
		}

		return 0;
	}

	int order = strcmp(key, (*bst)->key);

	if (order == 0) {
		(*bst)->value = value;
		return 0;
	} else if (order < 0) {
		return put_helper(&(*bst)->left, key, value);
	} else {
		return put_helper(&(*bst)->right, key, value);
	}
}

error_t bst_put(bst_t* bst, const char* key, department_t* value) {
	if (!bst || !key) return ERROR_INVALID_PARAMETER;

	return put_helper(&bst, key, value);
}

size_t bst_size(const bst_t* bst) {
	if (!bst) return 0;
	return 1 + bst_size(bst->left) + bst_size(bst->right);
}
