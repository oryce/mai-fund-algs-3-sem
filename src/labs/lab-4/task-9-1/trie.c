#include "trie.h"

// =============================================================================
// VTable functions
// =============================================================================

static void* vt_create(void) { return (void*)trie_create(); }

static void vt_destroy(void* storage) { trie_destroy((trie_t*)storage); }

static department_t* vt_get(const void* storage, const char* key) {
	return trie_get((trie_t*)storage, key);
}

static error_t vt_put(void* storage, const char* key, department_t* value) {
	return trie_put((trie_t*)storage, key, value);
}

const storage_vtable_t TRIE_VTABLE = {&vt_create, &vt_destroy, &vt_get,
                                      &vt_put};

// =============================================================================
// Utility functions
// =============================================================================

static trie_node_t* node_create() {
	return (trie_node_t*)calloc(1, sizeof(trie_node_t));
}

static void node_destroy(trie_node_t* node) {
	if (!node) return;

	for (size_t i = 0; i != TRIE_ALPHABET_SIZE; ++i) {
		if (node->children[i]) node_destroy(node->children[i]);
	}

	free(node);
}

static int char_idx(char c) {
	if (c >= 'A' && c <= 'Z') {
		return c - 'A';
	} else if (c >= 'a' && c <= 'z') {
		return 26 + 1 + (c - 'a');
	} else if (c >= '0' && c <= '9') {
		return 52 + 1 + (c - '0');
	} else if (c == '_') {
		return 63;
	} else {
		return -1;
	}
}

// =============================================================================
// Storage implementation
// =============================================================================

trie_t* trie_create() {
	trie_t* trie = (trie_t*)malloc(sizeof(trie_t));
	if (!trie) return NULL;

	trie->root = node_create();
	if (!trie->root) {
		free(trie);
		return NULL;
	}

	return trie;
}

void trie_destroy(trie_t* trie) {
	if (trie) {
		node_destroy(trie->root);
		free(trie);
	}
}

department_t* trie_get(const trie_t* trie, const char* key) {
	if (!trie || !key) return NULL;

	trie_node_t* node = trie->root;

	for (char* p = (char*)key; node && *p; ++p) {
		int idx = char_idx(*p);
		if (idx == -1) return NULL;

		node = node->children[idx];
	}

	return node && node->endOfWord ? node->value : NULL;
}

error_t trie_put(trie_t* trie, const char* key, department_t* value) {
	if (!trie || !key) return ERROR_INVALID_PARAMETER;

	trie_node_t* node = trie->root;

	for (char* p = (char*)key; *p; ++p) {
		int idx = char_idx(*p);
		if (idx == -1) return ERROR_TRIE_NOT_IN_ALPHABET;

		trie_node_t** next = &node->children[idx];

		if (*next == NULL) {
			*next = node_create();
			if (*next == NULL) return ERROR_OUT_OF_MEMORY;
		}

		node = *next;
	}

	node->endOfWord = true;
	node->value = value;

	return 0;
}
