#include "storage.h"

#include "bst.h"
#include "dynamic_array.h"
#include "hashtable.h"
#include "trie.h"

const storage_vtable_t* STORAGE_VTABLE_LOOKUP[] = {
    &BST_VTABLE, &DYNAMIC_ARRAY_VTABLE, &HASHTABLE_VTABLE, &TRIE_VTABLE};

storage_t* storage_create(storage_type_t type) {
	storage_t* storage = (storage_t*)malloc(sizeof(storage_t));
	if (!storage) return NULL;

	storage->type = type;
	storage->vtable = *STORAGE_VTABLE_LOOKUP[type];

	storage->storage = storage->vtable.create();
	if (!storage->storage) {
		free(storage);
		return NULL;
	}

	return storage;
}

void storage_destroy(storage_t* storage) {
	if (storage) {
		storage->vtable.destroy(storage->storage);
		free(storage);
	}
}

department_t* storage_get(const storage_t* storage, const char* key) {
	if (!storage) return NULL;
	return storage->vtable.get(storage->storage, key);
}

error_t storage_put(storage_t* storage, const char* key, department_t* value) {
	if (!storage) return ERROR_INVALID_PARAMETER;
	return storage->vtable.put(storage->storage, key, value);
}
