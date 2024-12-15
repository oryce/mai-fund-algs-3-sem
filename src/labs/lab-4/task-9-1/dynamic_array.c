#include "dynamic_array.h"

static const size_t MIN_CAPACITY = 4;

// =============================================================================
// VTable functions
// =============================================================================

static void* vt_create(void) { return (void*)dynamic_array_create(); }

static void vt_destroy(void* storage) {
	dynamic_array_destroy((dynamic_array_t*)storage);
}

static department_t* vt_get(const void* storage, const char* key) {
	return dynamic_array_get((dynamic_array_t*)storage, key);
}

static error_t vt_put(void* storage, const char* key, department_t* value) {
	return dynamic_array_put((dynamic_array_t*)storage, key, value);
}

const storage_vtable_t DYNAMIC_ARRAY_VTABLE = {&vt_create, &vt_destroy, &vt_get,
                                               &vt_put};

// =============================================================================
// Storage implementation
// =============================================================================

dynamic_array_t* dynamic_array_create() {
	dynamic_array_t* array = (dynamic_array_t*)malloc(sizeof(dynamic_array_t));
	if (!array) return NULL;

	array->buffer =
	    (array_entry_t*)malloc(sizeof(array_entry_t) * MIN_CAPACITY);
	if (!array->buffer) {
		free(array);
		return NULL;
	}

	array->size = 0;
	array->capacity = MIN_CAPACITY;

	return array;
}

void dynamic_array_destroy(dynamic_array_t* array) {
	if (array) {
		for (size_t i = 0; i != array->size; ++i) {
			free(array->buffer[i].key);
		}

		free(array->buffer);
		free(array);
	}
}

department_t* dynamic_array_get(const dynamic_array_t* array, const char* key) {
	if (!array || !key || !array->size) return NULL;

	size_t l = 0;
	size_t r = array->size - 1;

	while (l <= r) {
		size_t mid = (l + r) / 2;

		int order = strcmp(array->buffer[mid].key, key);

		if (order < 0) {
			l = mid + 1;
		} else if (order > 0) {
			r = mid - 1;
		} else {
			return array->buffer[mid].value;
		}
	}

	return NULL;
}

error_t dynamic_array_put(dynamic_array_t* array, const char* key,
                          department_t* value) {
	if (!array || !key) return ERROR_INVALID_PARAMETER;

	if (array->size == array->capacity) {
		array_entry_t* newBuffer = (array_entry_t*)realloc(
		    array->buffer, sizeof(array_entry_t) * (array->capacity * 2));
		if (!newBuffer) return ERROR_OUT_OF_MEMORY;

		array->buffer = newBuffer;
		array->capacity *= 2;
	}

	ssize_t l = 0;
	ssize_t r = (ssize_t)array->size - 1;

	while (l <= r) {
		size_t mid = (l + r) / 2;

		int order = strcmp(array->buffer[mid].key, key);

		if (order < 0) {
			l = mid + 1;
		} else if (order > 0) {
			r = mid - 1;
		} else {
			array->buffer[mid].value = value;  // Update value.
			return 0;
		}
	}

	// Shift elements from |l| to the right by one.
	memmove(array->buffer + l + 1, array->buffer + l,
	        sizeof(array_entry_t) * (array->size - l));

	// Insert new value.
	array->buffer[l].key = strdup(key);
	array->buffer[l].value = value;

	if (!array->buffer[l].key) {
		return ERROR_OUT_OF_MEMORY;
	}

	array->size++;

	return 0;
}

size_t dynamic_array_size(const dynamic_array_t* array) {
	return array ? array->size : SIZE_MAX;
}
