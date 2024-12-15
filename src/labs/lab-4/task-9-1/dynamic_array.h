#pragma once

#include "department.h"
#include "storage.h"

extern const storage_vtable_t DYNAMIC_ARRAY_VTABLE;

typedef struct array_entry {
	const char* key;
	department_t* value;
} array_entry_t;

typedef struct dynamic_array {
	array_entry_t* buffer;
	size_t size;
	size_t capacity;
} dynamic_array_t;

dynamic_array_t* dynamic_array_create();

void dynamic_array_destroy(dynamic_array_t* array);

department_t* dynamic_array_get(const dynamic_array_t* array, const char* key);

error_t dynamic_array_put(dynamic_array_t* array, const char* key,
                          department_t* value);

size_t dynamic_array_size(const dynamic_array_t* array);
