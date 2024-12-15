#include "hashtable.h"

#include <stdlib.h>
#include <string.h>

const size_t HASHTABLE_DEF_CAPACITY = 64;
const float HASHTABLE_DEF_LOAD_FACTOR = 0.5f;

// =============================================================================
// VTable functions
// =============================================================================

static void* vt_create(void) {
	return (void*)hashtable_create(HASHTABLE_DEF_CAPACITY,
	                               HASHTABLE_DEF_LOAD_FACTOR);
}

static void vt_destroy(void* storage) {
	hashtable_destroy((hashtable_t*)storage);
}

static department_t* vt_get(const void* storage, const char* key) {
	return hashtable_get((hashtable_t*)storage, key);
}

static error_t vt_put(void* storage, const char* key, department_t* value) {
	return hashtable_put((hashtable_t*)storage, key, value);
}

const storage_vtable_t HASHTABLE_VTABLE = {&vt_create, &vt_destroy, &vt_get,
                                           &vt_put};

// =============================================================================
// Utility functions
// =============================================================================

static uint64_t hash_fnv1a(const char* key) {
	static const uint64_t FNV_offset_basis = 0xcbf29ce484222325;
	static const uint64_t FNV_prime = 0x100000001b3;

	uint64_t hash = FNV_offset_basis;

	for (const char* p = key; *p; ++p) {
		hash ^= (unsigned char)*p;
		hash *= FNV_prime;
	}

	return hash;
}

static hashtable_entry_t* create_entry(const char* key, department_t* value) {
	hashtable_entry_t* entry =
	    (hashtable_entry_t*)malloc(sizeof(hashtable_entry_t));
	if (!entry) return NULL;

	entry->key = strdup(key);
	if (!entry->key) {
		free(entry);
		return NULL;
	}

	entry->value = value;
	entry->prev = entry;
	entry->next = entry;

	return entry;
}

static error_t set_entry(hashtable_entry_t** table, size_t capacity,
                         const char* key, department_t* value, size_t* size) {
	if (!table || !key) return ERROR_INVALID_PARAMETER;

	uint64_t hash = hash_fnv1a(key);
	size_t index = hash % capacity;

	hashtable_entry_t* entry = create_entry(key, value);
	if (!entry) return ERROR_OUT_OF_MEMORY;

	if (!table[index]) {
		table[index] = entry;
	} else {
		hashtable_entry_t* head = table[index];
		hashtable_entry_t *prev0 = head->prev, *next0 = head->next;

		head->prev = entry;
		entry->next = head;
		entry->prev = prev0;
		next0->next = entry;
	}

	if (size) {
		++(*size);
	}

	return 0;
}

// =============================================================================
// Storage implementation
// =============================================================================

hashtable_t* hashtable_create(size_t capacity, float loadFactor) {
	hashtable_t* ht = (hashtable_t*)malloc(sizeof(hashtable_t));
	if (!ht) return NULL;

	ht->table = calloc(capacity, sizeof(hashtable_entry_t));
	if (!ht->table) {
		free(ht);
		return NULL;
	}

	ht->size = 0;
	ht->capacity = capacity;
	ht->loadFactor = loadFactor;

	return ht;
}

void hashtable_destroy(hashtable_t* ht) {
	if (!ht) return;

	for (size_t i = 0; i != ht->capacity; ++i) {
		if (ht->table[i]) {
			free(ht->table[i]->key);
			free(ht->table[i]);
		}
	}

	free(ht->table);
	free(ht);
}

department_t* hashtable_get(const hashtable_t* ht, const char* key) {
	if (!ht || !key) return NULL;

	uint64_t hash = hash_fnv1a(key);
	size_t index = hash % ht->capacity;

	const hashtable_entry_t* head = ht->table[index];
	if (!head) return NULL;

	const hashtable_entry_t* cur = head;
	do {
		if (strcmp(cur->key, key) == 0) {
			return cur->value;
		}
		cur = cur->next;
	} while (cur != head);

	return NULL;
}

error_t hashtable_put(hashtable_t* ht, const char* key, department_t* value) {
	if (!ht || !key) return ERROR_INVALID_PARAMETER;

	float loadFactor = (float)ht->size / (float)ht->capacity;

	if (loadFactor > ht->loadFactor) {
		// Expand the hash table.
		size_t newCapacity = ht->capacity * 2;

		hashtable_entry_t** newTable =
		    calloc(newCapacity, sizeof(hashtable_entry_t*));
		if (!newTable) return ERROR_OUT_OF_MEMORY;

		for (size_t i = 0; i != ht->capacity; ++i) {
			const hashtable_entry_t* head = ht->table[i];
			if (!head) continue;

			const hashtable_entry_t* cur = head;
			do {
				error_t error = set_entry(newTable, newCapacity, cur->key,
				                          cur->value, NULL);
				if (error) {
					free(newTable);
					return error;
				}
				cur = cur->next;
			} while (cur != head);
		}

		free(ht->table);
		ht->table = newTable;
		ht->capacity = newCapacity;
	}

	return set_entry(ht->table, ht->capacity, key, value, &ht->size);
}

size_t hashtable_size(const hashtable_t* ht) { return ht->size; }
