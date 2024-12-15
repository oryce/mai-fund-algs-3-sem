#pragma once

#include "department.h"

typedef struct storage_vtable {
	void* (*create)(void);
	void (*destroy)(void* storage);
	department_t* (*get)(const void* storage, const char* key);
	error_t (*put)(void* storage, const char* key, department_t* value);
} storage_vtable_t;

extern const storage_vtable_t* STORAGE_VTABLE_LOOKUP[];

typedef enum storage_type {
	STORAGE_BST,
	STORAGE_DYNAMIC_ARRAY,
	STORAGE_HASHTABLE,
	STORAGE_TRIE
} storage_type_t;

typedef struct storage {
	void* storage;
	storage_type_t type;
	storage_vtable_t vtable;
} storage_t;

storage_t* storage_create(storage_type_t type);

void storage_destroy(storage_t* storage);

department_t* storage_get(const storage_t* storage, const char* key);

error_t storage_put(storage_t* storage, const char* key, department_t* value);
