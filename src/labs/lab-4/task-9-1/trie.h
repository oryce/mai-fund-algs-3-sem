#pragma once

#include <stdbool.h>
#include <stddef.h>

#include "department.h"
#include "storage.h"

#define TRIE_ALPHABET_SIZE 63  // '[a-zA-Z0-9_]'

#define ERROR_TRIE_NOT_IN_ALPHABET 0x20000001

extern const storage_vtable_t TRIE_VTABLE;

typedef struct trie_node {
	struct trie_node* children[TRIE_ALPHABET_SIZE];
	bool endOfWord;
	department_t* value;
} trie_node_t;

typedef struct trie {
	trie_node_t* root;
} trie_t;

trie_t* trie_create();

void trie_destroy(trie_t* trie);

department_t* trie_get(const trie_t* trie, const char* key);

error_t trie_put(trie_t* trie, const char* key, department_t* value);
