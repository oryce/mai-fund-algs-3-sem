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

#define DEFINE_VECTOR(VECTOR_T, TYPE_T, TYPE)                         \
	typedef struct vector_##TYPE {                                    \
		TYPE_T* buffer;                                               \
		size_t size;                                                  \
		size_t capacity;                                              \
		struct vector_helpers helpers;                                \
	} VECTOR_T;                                                       \
                                                                      \
	VECTOR_T vector_##TYPE##_create(void);                            \
                                                                      \
	VECTOR_T vector_##TYPE##_create_with_capacity(size_t capacity);   \
                                                                      \
	void vector_##TYPE##_destroy(VECTOR_T*);                          \
                                                                      \
	bool vector_##TYPE##_ensure_capacity(VECTOR_T*, size_t capacity); \
                                                                      \
	bool vector_##TYPE##_push_back(VECTOR_T*, TYPE_T value);          \
                                                                      \
	bool vector_##TYPE##_pop_back(VECTOR_T*, TYPE_T* out);            \
                                                                      \
	TYPE_T* vector_##TYPE##_get(VECTOR_T*, size_t i);                 \
                                                                      \
	bool vector_##TYPE##_remove(VECTOR_T*, size_t i, TYPE_T* out);    \
                                                                      \
	bool vector_##TYPE##_remove_item(VECTOR_T*, TYPE_T value);        \
                                                                      \
	bool vector_##TYPE##_clear(VECTOR_T*);                            \
                                                                      \
	size_t vector_##TYPE##_index_of(VECTOR_T*, TYPE_T value);         \
                                                                      \
	size_t vector_##TYPE##_size(VECTOR_T*);                           \
                                                                      \
	bool vector_##TYPE##_is_empty(VECTOR_T*);                         \
                                                                      \
	const TYPE_T* vector_##TYPE##_to_array(VECTOR_T*);                \
                                                                      \
	bool vector_##TYPE##_swap(VECTOR_T*, size_t i1, size_t i2);       \
                                                                      \
	bool vector_##TYPE##_sort(VECTOR_T*);

#define IMPL_VECTOR(VECTOR_T, TYPE_T, TYPE, HELPERS)                                                            \
	bool vector_##TYPE##_resize(VECTOR_T* v, size_t capacity) {                                                 \
		if (!v) return false;                                                                                   \
                                                                                                                \
		TYPE_T* newBuffer = realloc(v->buffer, capacity * sizeof(TYPE_T));                                      \
		if (newBuffer == NULL) return false;                                                                    \
                                                                                                                \
		v->buffer = newBuffer;                                                                                  \
		v->capacity = capacity;                                                                                 \
		return true;                                                                                            \
	}                                                                                                           \
                                                                                                                \
	bool vector_##TYPE##_shrink_one(VECTOR_T* v) {                                                              \
		if (!v) return false;                                                                                   \
                                                                                                                \
		if ((v->size - 1) > VECTOR_MIN_CAPACITY && (v->size - 1) <= v->capacity * 4 / 9) {                      \
			bool resized = vector_##TYPE##_resize(v, v->capacity * 2 / 3);                                      \
			if (!resized) return false;                                                                         \
		}                                                                                                       \
                                                                                                                \
		v->size--;                                                                                              \
		return true;                                                                                            \
	}                                                                                                           \
                                                                                                                \
	VECTOR_T vector_##TYPE##_create(void) { return vector_##TYPE##_create_with_capacity(VECTOR_MIN_CAPACITY); } \
                                                                                                                \
	VECTOR_T vector_##TYPE##_create_with_capacity(size_t capacity) {                                            \
		/* allocate later, when a push occurs */                                                                \
		VECTOR_T v = {.buffer = NULL, .size = 0, .capacity = capacity, .helpers = HELPERS};                     \
		return v;                                                                                               \
	}                                                                                                           \
                                                                                                                \
	void vector_##TYPE##_destroy(VECTOR_T* v) {                                                                 \
		if (!v) return;                                                                                         \
		if (v->buffer != NULL) free(v->buffer);                                                                 \
                                                                                                                \
		v->buffer = NULL;                                                                                       \
		v->size = 0;                                                                                            \
		v->capacity = 0;                                                                                        \
	}                                                                                                           \
                                                                                                                \
	bool vector_##TYPE##_ensure_capacity(VECTOR_T* v, size_t capacity) {                                        \
		if (!v)                                                                                                 \
			return false;                                                                                       \
		else if (v->capacity >= capacity)                                                                       \
			return true;                                                                                        \
		return vector_##TYPE##_resize(v, capacity);                                                             \
	}                                                                                                           \
                                                                                                                \
	bool vector_##TYPE##_push_back(VECTOR_T* v, TYPE_T value) {                                                 \
		if (!v) return false;                                                                                   \
                                                                                                                \
		/* if the vec is not initialized, allocate the buffer */                                                \
		if (v->buffer == NULL) {                                                                                \
			if (v->capacity == 0) return false;                                                                 \
                                                                                                                \
			v->buffer = calloc(v->capacity, sizeof(TYPE_T));                                                    \
			if (v->buffer == NULL) return false; /* allocation failed */                                        \
		}                                                                                                       \
                                                                                                                \
		if (v->size == v->capacity) {                                                                           \
			bool resized = vector_##TYPE##_ensure_capacity(v, v->capacity * 3 / 2);                             \
			if (!resized) return false;                                                                         \
		}                                                                                                       \
                                                                                                                \
		v->buffer[v->size++] = value;                                                                           \
		return true;                                                                                            \
	}                                                                                                           \
                                                                                                                \
	bool vector_##TYPE##_pop_back(VECTOR_T* v, TYPE_T* out) {                                                   \
		if (!v || !out || v->size == 0) return false;                                                           \
                                                                                                                \
		TYPE_T value = v->buffer[v->size - 1];                                                                  \
		if (!vector_##TYPE##_shrink_one(v)) return false;                                                       \
                                                                                                                \
		*out = value;                                                                                           \
		return true;                                                                                            \
	}                                                                                                           \
                                                                                                                \
	TYPE_T* vector_##TYPE##_get(VECTOR_T* v, size_t i) {                                                        \
		if (!v || i >= v->size) return NULL;                                                                    \
		return &v->buffer[i];                                                                                   \
	}                                                                                                           \
                                                                                                                \
	bool vector_##TYPE##_remove(VECTOR_T* v, size_t i, TYPE_T* out) {                                           \
		if (!v || i >= v->size) return false;                                                                   \
                                                                                                                \
		TYPE_T value = v->buffer[i];                                                                            \
		if (out) *out = value;                                                                                  \
                                                                                                                \
		memmove(v->buffer + i, v->buffer + i + 1, (v->size - i - 1) * sizeof(TYPE_T));                          \
		if (!vector_##TYPE##_shrink_one(v)) return false;                                                       \
                                                                                                                \
		return true;                                                                                            \
	}                                                                                                           \
                                                                                                                \
	bool vector_##TYPE##_remove_item(VECTOR_T* v, TYPE_T value) {                                               \
		if (!v) return false;                                                                                   \
		return vector_##TYPE##_remove(v, vector_##TYPE##_index_of(v, value), NULL);                             \
	}                                                                                                           \
                                                                                                                \
	bool vector_##TYPE##_clear(VECTOR_T* v) {                                                                   \
		if (!v || !vector_##TYPE##_resize(v, 4)) return false;                                                  \
                                                                                                                \
		v->size = 0;                                                                                            \
		return true;                                                                                            \
	}                                                                                                           \
                                                                                                                \
	size_t vector_##TYPE##_index_of(VECTOR_T* v, TYPE_T value) {                                                \
		if (!v || v->helpers.comparator == NULL) return SIZE_MAX;                                               \
                                                                                                                \
		for (size_t i = 0; i != v->size; i++) {                                                                 \
			int order = v->helpers.comparator(&v->buffer[i], &value);                                           \
			if (order == 0) return (ssize_t)i;                                                                  \
		}                                                                                                       \
                                                                                                                \
		return SIZE_MAX;                                                                                        \
	}                                                                                                           \
                                                                                                                \
	size_t vector_##TYPE##_size(vector_##TYPE##_t* v) { return v->size; }                                       \
                                                                                                                \
	bool vector_##TYPE##_is_empty(vector_##TYPE##_t* v) { return v->size != 0; }                                \
                                                                                                                \
	const TYPE_T* vector_##TYPE##_to_array(vector_##TYPE##_t* v) { return (const TYPE_T*)v->buffer; }           \
                                                                                                                \
	bool vector_##TYPE##_swap(vector_##TYPE##_t* v, size_t i1, size_t i2) {                                     \
		if (!v || i1 >= v->size || i2 >= v->size) return false;                                                 \
                                                                                                                \
		TYPE_T temp = v->buffer[i1];                                                                            \
		v->buffer[i1] = v->buffer[i2];                                                                          \
		v->buffer[i2] = temp;                                                                                   \
                                                                                                                \
		return true;                                                                                            \
	}                                                                                                           \
                                                                                                                \
	bool vector_##TYPE##_sort(vector_##TYPE##_t* v) {                                                           \
		if (!v || v->helpers.comparator == NULL) return false;                                                  \
                                                                                                                \
		bool result = true;                                                                                     \
		deque_idx_t lefts = deque_idx_create();  /* Left pair elements */                                       \
		deque_idx_t rights = deque_idx_create(); /* Right pair elements */                                      \
                                                                                                                \
		result = deque_idx_push_front(&lefts, 0);                                                               \
		if (!result) goto cleanup;                                                                              \
		result = deque_idx_push_front(&rights, v->size - 1);                                                    \
		if (!result) goto cleanup;                                                                              \
                                                                                                                \
		do {                                                                                                    \
			size_t l, r;                                                                                        \
                                                                                                                \
			result = deque_idx_pop_front(&lefts, &l);                                                           \
			if (!result) goto cleanup;                                                                          \
			result = deque_idx_pop_front(&rights, &r);                                                          \
			if (!result) goto cleanup;                                                                          \
                                                                                                                \
			do {                                                                                                \
				TYPE_T* pivot = vector_##TYPE##_get(v, (l + r) / 2);                                            \
				size_t i = l;                                                                                   \
				size_t j = r;                                                                                   \
                                                                                                                \
				do {                                                                                            \
					while (v->helpers.comparator(&v->buffer[i], pivot) == -1) i++;                              \
					while (v->helpers.comparator(&v->buffer[j], pivot) == 1) j--;                               \
                                                                                                                \
					if (i <= j) {                                                                               \
						vector_##TYPE##_swap(v, i, j);                                                          \
						++i;                                                                                    \
                                                                                                                \
						if (j == 0) break; /* Prevent underflow with size_t */                                  \
						--j;                                                                                    \
					}                                                                                           \
				} while (i <= j);                                                                               \
                                                                                                                \
				if (i < r) {                                                                                    \
					result = deque_idx_push_front(&lefts, i);                                                   \
					if (!result) goto cleanup;                                                                  \
					result = deque_idx_push_front(&rights, r);                                                  \
					if (!result) goto cleanup;                                                                  \
				}                                                                                               \
                                                                                                                \
				r = j;                                                                                          \
			} while (l < r);                                                                                    \
		} while (!deque_idx_is_empty(&lefts) && !deque_idx_is_empty(&rights));                                  \
                                                                                                                \
	cleanup:                                                                                                    \
		deque_idx_destroy(&lefts);                                                                              \
		deque_idx_destroy(&rights);                                                                             \
		return result;                                                                                          \
	}

DEFINE_VECTOR(vector_i8_t, int8_t, i8)
DEFINE_VECTOR(vector_i16_t, int16_t, i16)
DEFINE_VECTOR(vector_i32_t, int32_t, i32)
DEFINE_VECTOR(vector_i64_t, int64_t, i64)
DEFINE_VECTOR(vector_u8_t, uint8_t, u8)
DEFINE_VECTOR(vector_u16_t, uint16_t, u16)
DEFINE_VECTOR(vector_u32_t, uint32_t, u32)
DEFINE_VECTOR(vector_u64_t, uint64_t, u64)
DEFINE_VECTOR(vector_flt_t, float, flt)
DEFINE_VECTOR(vector_dbl_t, double, dbl)
DEFINE_VECTOR(vector_ptr_t, void*, ptr)
