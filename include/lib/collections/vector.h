#pragma once

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#include "lib/collections/deque.h"

extern const size_t VECTOR_MIN_CAPACITY;

struct vector_helpers {
	int (*comparator)(const void* p1, const void* p2);
};

#define DEFINE_VECTOR(TYPE, TNAME)                                                               \
	typedef struct vector_##TNAME {                                                              \
		TYPE* buffer;                                                                            \
		size_t size;                                                                             \
		size_t capacity;                                                                         \
		struct vector_helpers helpers;                                                           \
	} vector_##TNAME##_t;                                                                        \
                                                                                                 \
	vector_##TNAME##_t vector_##TNAME##_create(void);                                            \
                                                                                                 \
	vector_##TNAME##_t vector_##TNAME##_create_with_capacity(size_t capacity);                   \
                                                                                                 \
	void vector_##TNAME##_destroy(vector_##TNAME##_t*);                                          \
                                                                                                 \
	bool vector_##TNAME##_ensure_capacity(vector_##TNAME##_t*, size_t capacity);                 \
                                                                                                 \
	bool vector_##TNAME##_push_back(vector_##TNAME##_t*, TYPE value);                            \
                                                                                                 \
	TYPE vector_##TNAME##_pop_back(vector_##TNAME##_t*);                                         \
                                                                                                 \
	TYPE vector_##TNAME##_get(vector_##TNAME##_t*, size_t index);                                \
                                                                                                 \
	TYPE vector_##TNAME##_remove(vector_##TNAME##_t*, size_t index);                             \
                                                                                                 \
	TYPE vector_##TNAME##_remove_item(vector_##TNAME##_t*, TYPE value);                          \
                                                                                                 \
	bool vector_##TNAME##_clear(vector_##TNAME##_t*);                                            \
                                                                                                 \
	size_t vector_##TNAME##_index_of(vector_##TNAME##_t*, TYPE value);                           \
                                                                                                 \
	inline static size_t vector_##TNAME##_size(vector_##TNAME##_t* v) { return v->size; }        \
                                                                                                 \
	inline static bool vector_##TNAME##_is_empty(vector_##TNAME##_t* v) { return v->size == 0; } \
                                                                                                 \
	inline static TYPE* vector_##TNAME##_to_array(vector_##TNAME##_t* v) { return v->buffer; }   \
                                                                                                 \
	bool vector_##TNAME##_swap(vector_##TNAME##_t*, size_t i1, size_t i2);                       \
                                                                                                 \
	bool vector_##TNAME##_sort(vector_##TNAME##_t*);

#define IMPL_VECTOR(VTYPE, TYPE, TNAME, TDEF, HELPERS)                                                         \
	bool vector_##TNAME##_resize(VTYPE* v, size_t capacity) {                                                  \
		if (v == NULL) return false;                                                                           \
                                                                                                               \
		TYPE* newBuffer = realloc(v->buffer, capacity * sizeof(TYPE));                                         \
		if (newBuffer == NULL) return false;                                                                   \
                                                                                                               \
		v->buffer = newBuffer;                                                                                 \
		v->capacity = capacity;                                                                                \
		return true;                                                                                           \
	}                                                                                                          \
                                                                                                               \
	bool vector_##TNAME##_shrink_one(VTYPE* v) {                                                               \
		if (v == NULL) return false;                                                                           \
                                                                                                               \
		if ((v->size - 1) > VECTOR_MIN_CAPACITY && (v->size - 1) <= v->capacity * 4 / 9) {                     \
			bool resized = vector_##TNAME##_resize(v, v->capacity * 2 / 3);                                    \
			if (!resized) return false;                                                                        \
		}                                                                                                      \
                                                                                                               \
		v->size--;                                                                                             \
		return true;                                                                                           \
	}                                                                                                          \
                                                                                                               \
	VTYPE vector_##TNAME##_create(void) { return vector_##TNAME##_create_with_capacity(VECTOR_MIN_CAPACITY); } \
                                                                                                               \
	VTYPE vector_##TNAME##_create_with_capacity(size_t capacity) {                                             \
		/* allocate later, when a push occurs */                                                               \
		VTYPE v = {.buffer = NULL, .size = 0, .capacity = capacity, .helpers = HELPERS};                       \
		return v;                                                                                              \
	}                                                                                                          \
                                                                                                               \
	void vector_##TNAME##_destroy(VTYPE* v) {                                                                  \
		if (v == NULL) return;                                                                                 \
                                                                                                               \
		if (v->buffer != NULL) {                                                                               \
			free(v->buffer);                                                                                   \
		}                                                                                                      \
                                                                                                               \
		v->buffer = NULL;                                                                                      \
		v->size = 0;                                                                                           \
		v->capacity = 0;                                                                                       \
	}                                                                                                          \
                                                                                                               \
	bool vector_##TNAME##_ensure_capacity(VTYPE* v, size_t capacity) {                                         \
		if (v == NULL)                                                                                         \
			return false;                                                                                      \
		else if (v->capacity >= capacity)                                                                      \
			return true;                                                                                       \
		return vector_##TNAME##_resize(v, capacity);                                                           \
	}                                                                                                          \
                                                                                                               \
	bool vector_##TNAME##_push_back(VTYPE* v, TYPE value) {                                                    \
		if (v == NULL) return false;                                                                           \
                                                                                                               \
		/* if the vec is not initialized, allocate the buffer */                                               \
		if (v->buffer == NULL) {                                                                               \
			if (v->capacity == 0) return false;                                                                \
                                                                                                               \
			v->buffer = calloc(v->capacity, sizeof(TYPE));                                                     \
			if (v->buffer == NULL) return false; /* allocation failed */                                       \
		}                                                                                                      \
                                                                                                               \
		if (v->size == v->capacity) {                                                                          \
			bool resized = vector_##TNAME##_ensure_capacity(v, v->capacity * 3 / 2);                           \
			if (!resized) return false;                                                                        \
		}                                                                                                      \
                                                                                                               \
		v->buffer[v->size++] = value;                                                                          \
		return true;                                                                                           \
	}                                                                                                          \
                                                                                                               \
	TYPE vector_##TNAME##_pop_back(VTYPE* v) {                                                                 \
		if (v == NULL || v->size == 0) return TDEF;                                                            \
                                                                                                               \
		TYPE value = v->buffer[v->size - 1];                                                                   \
		if (!vector_##TNAME##_shrink_one(v)) return TDEF;                                                      \
                                                                                                               \
		return value;                                                                                          \
	}                                                                                                          \
                                                                                                               \
	TYPE vector_##TNAME##_get(VTYPE* v, size_t index) {                                                        \
		if (v == NULL || index >= v->size) return TDEF;                                                        \
		return v->buffer[index];                                                                               \
	}                                                                                                          \
                                                                                                               \
	TYPE vector_##TNAME##_remove(VTYPE* v, size_t index) {                                                     \
		if (v == NULL || index >= v->size) return TDEF;                                                        \
                                                                                                               \
		TYPE value = v->buffer[index];                                                                         \
		memmove(v->buffer + index, v->buffer + index + 1, (v->size - index - 1) * sizeof(TYPE));               \
                                                                                                               \
		if (!vector_##TNAME##_shrink_one(v)) return TDEF;                                                      \
                                                                                                               \
		return value;                                                                                          \
	}                                                                                                          \
                                                                                                               \
	TYPE vector_##TNAME##_remove_item(VTYPE* v, TYPE value) {                                                  \
		if (v == NULL) return TDEF;                                                                            \
		return vector_##TNAME##_remove(v, vector_##TNAME##_index_of(v, value));                                \
	}                                                                                                          \
                                                                                                               \
	bool vector_##TNAME##_clear(VTYPE* v) {                                                                    \
		if (v == NULL || !vector_##TNAME##_resize(v, 4)) return false;                                         \
                                                                                                               \
		v->size = 0;                                                                                           \
		return true;                                                                                           \
	}                                                                                                          \
                                                                                                               \
	size_t vector_##TNAME##_index_of(VTYPE* v, TYPE value) {                                                   \
		if (v == NULL || v->helpers.comparator == NULL) return SIZE_MAX;                                       \
                                                                                                               \
		for (size_t i = 0; i != v->size; i++) {                                                                \
			int order = v->helpers.comparator(&v->buffer[i], &value);                                          \
			if (order == 0) return (ssize_t)i;                                                                 \
		}                                                                                                      \
                                                                                                               \
		return SIZE_MAX;                                                                                       \
	}                                                                                                          \
                                                                                                               \
	bool vector_##TNAME##_swap(vector_##TNAME##_t* v, size_t i1, size_t i2) {                                  \
		if (v == NULL || i1 >= v->size || i2 >= v->size) return false;                                         \
                                                                                                               \
		TYPE temp = v->buffer[i1];                                                                             \
		v->buffer[i1] = v->buffer[i2];                                                                         \
		v->buffer[i2] = temp;                                                                                  \
                                                                                                               \
		return true;                                                                                           \
	}                                                                                                          \
                                                                                                               \
	bool vector_##TNAME##_sort(vector_##TNAME##_t* v) {                                                        \
		if (v == NULL || v->helpers.comparator == NULL) return false;                                          \
                                                                                                               \
		bool result = true;                                                                                    \
		deque_idx_t lefts = deque_idx_create();  /* Left pair elements */                                      \
		deque_idx_t rights = deque_idx_create(); /* Right pair elements */                                     \
                                                                                                               \
		result = deque_idx_push_front(&lefts, 0);                                                              \
		if (!result) goto cleanup;                                                                             \
		result = deque_idx_push_front(&rights, v->size - 1);                                                   \
		if (!result) goto cleanup;                                                                             \
                                                                                                               \
		do {                                                                                                   \
			size_t l, r;                                                                                \
                                                                                                               \
			result = deque_idx_pop_front(&lefts, &l);                                                          \
			if (!result) goto cleanup;                                                                         \
			result = deque_idx_pop_front(&rights, &r);                                                         \
			if (!result) goto cleanup;                                                                         \
                                                                                                               \
			do {                                                                                               \
				TYPE pivot = vector_##TNAME##_get(v, (l + r) / 2);                                             \
				size_t i = l;                                                                                  \
				size_t j = r;                                                                                  \
                                                                                                               \
				do {                                                                                           \
					while (v->helpers.comparator(&v->buffer[i], &pivot) == -1) i++;                            \
					while (v->helpers.comparator(&v->buffer[j], &pivot) == 1) j--;                             \
                                                                                                               \
					if (i <= j) {                                                                              \
						vector_##TNAME##_swap(v, i, j);                                                        \
						++i;                                                                                   \
                                                                                                               \
						if (j == 0) break; /* Prevent underflow with size_t */                                 \
						--j;                                                                                   \
					}                                                                                          \
				} while (i <= j);                                                                              \
                                                                                                               \
				if (i < r) {                                                                                   \
					result = deque_idx_push_front(&lefts, i);                                                  \
					if (!result) goto cleanup;                                                                 \
					result = deque_idx_push_front(&rights, r);                                                 \
					if (!result) goto cleanup;                                                                 \
				}                                                                                              \
                                                                                                               \
				r = j;                                                                                         \
			} while (l < r);                                                                                   \
		} while (!deque_idx_is_empty(&lefts) && !deque_idx_is_empty(&rights));                                 \
                                                                                                               \
	cleanup:                                                                                                   \
		deque_idx_destroy(&lefts);                                                                             \
		deque_idx_destroy(&rights);                                                                            \
		return result;                                                                                         \
	}

DEFINE_VECTOR(int8_t, i8)
DEFINE_VECTOR(int16_t, i16)
DEFINE_VECTOR(int32_t, i32)
DEFINE_VECTOR(int64_t, i64)
DEFINE_VECTOR(uint8_t, u8)
DEFINE_VECTOR(uint16_t, u16)
DEFINE_VECTOR(uint32_t, u32)
DEFINE_VECTOR(uint64_t, u64)
DEFINE_VECTOR(float, flt)
DEFINE_VECTOR(double, dbl)
DEFINE_VECTOR(void*, ptr)
