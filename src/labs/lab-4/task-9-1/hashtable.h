#pragma once

#include <stddef.h>
#include <stdint.h>

#include "department.h"
#include "lib/error.h"
#include "storage.h"

extern const storage_vtable_t HASHTABLE_VTABLE;
extern const size_t HASHTABLE_DEF_CAPACITY;
extern const float HASHTABLE_DEF_LOAD_FACTOR;

typedef struct hashtable_entry {
	const char* key;
	department_t* value;
	struct hashtable_entry* prev;
	struct hashtable_entry* next;
} hashtable_entry_t;

typedef struct hashtable {
	hashtable_entry_t** table;
	size_t size;
	size_t capacity;
	float loadFactor;
} hashtable_t;

hashtable_t* hashtable_create(size_t capacity, float loadFactor);

void hashtable_destroy(hashtable_t* ht);

department_t* hashtable_get(const hashtable_t* ht, const char* key);

error_t hashtable_put(hashtable_t* ht, const char* key, department_t* value);

size_t hashtable_size(const hashtable_t* ht);
