#pragma once

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

extern const size_t VECTOR_MIN_CAPACITY;

struct vector_utils {
	int (*comp)(const void* p1, const void* p2);
};

#define DEFINE_VECTOR(VECTOR_T, TYPE_T, TYPE)                         \
	typedef struct vector_##TYPE {                                    \
		TYPE_T* buffer;                                               \
		size_t size;                                                  \
		size_t capacity;                                              \
		struct vector_utils utils;                                    \
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
	bool vector_##TYPE##_insert(VECTOR_T*, size_t idx, TYPE_T value); \
                                                                      \
	TYPE_T* vector_##TYPE##_get(const VECTOR_T*, size_t i);           \
                                                                      \
	bool vector_##TYPE##_remove(VECTOR_T*, size_t i, TYPE_T* out);    \
                                                                      \
	bool vector_##TYPE##_remove_item(VECTOR_T*, TYPE_T value);        \
                                                                      \
	bool vector_##TYPE##_clear(VECTOR_T*);                            \
                                                                      \
	size_t vector_##TYPE##_index_of(const VECTOR_T*, TYPE_T value);   \
                                                                      \
	size_t vector_##TYPE##_size(const VECTOR_T*);                     \
                                                                      \
	bool vector_##TYPE##_is_empty(const VECTOR_T*);                   \
                                                                      \
	const TYPE_T* vector_##TYPE##_to_array(const VECTOR_T*);          \
                                                                      \
	bool vector_##TYPE##_sort(VECTOR_T*);                             \
                                                                      \
	bool vector_##TYPE##_dup(const VECTOR_T* src, VECTOR_T* dst);

#define IMPL_VECTOR(VECTOR_T, TYPE_T, TYPE, UTILS)                             \
	bool vector_##TYPE##_resize(VECTOR_T* v, size_t capacity) {                \
		if (!v) return false;                                                  \
                                                                               \
		TYPE_T* newBuffer =                                                    \
		    (TYPE_T*)realloc(v->buffer, capacity * sizeof(TYPE_T));            \
		if (newBuffer == NULL) return false;                                   \
                                                                               \
		v->buffer = newBuffer;                                                 \
		v->capacity = capacity;                                                \
		return true;                                                           \
	}                                                                          \
                                                                               \
	bool vector_##TYPE##_shrink_one(VECTOR_T* v) {                             \
		if (!v) return false;                                                  \
                                                                               \
		if ((v->size - 1) > VECTOR_MIN_CAPACITY &&                             \
		    (v->size - 1) <= v->capacity * 4 / 9) {                            \
			bool resized = vector_##TYPE##_resize(v, v->capacity * 2 / 3);     \
			if (!resized) return false;                                        \
		}                                                                      \
                                                                               \
		v->size--;                                                             \
		return true;                                                           \
	}                                                                          \
                                                                               \
	VECTOR_T vector_##TYPE##_create(void) {                                    \
		return vector_##TYPE##_create_with_capacity(VECTOR_MIN_CAPACITY);      \
	}                                                                          \
                                                                               \
	VECTOR_T vector_##TYPE##_create_with_capacity(size_t capacity) {           \
		/* allocate later, when a push occurs */                               \
		VECTOR_T v = {                                                         \
		    .buffer = NULL, .size = 0, .capacity = capacity, .utils = UTILS};  \
		return v;                                                              \
	}                                                                          \
                                                                               \
	void vector_##TYPE##_destroy(VECTOR_T* v) {                                \
		if (!v) return;                                                        \
		if (v->buffer != NULL) free(v->buffer);                                \
                                                                               \
		v->buffer = NULL;                                                      \
		v->size = 0;                                                           \
		v->capacity = 0;                                                       \
	}                                                                          \
                                                                               \
	bool vector_##TYPE##_ensure_capacity(VECTOR_T* v, size_t capacity) {       \
		if (!v)                                                                \
			return false;                                                      \
		else if (v->capacity >= capacity)                                      \
			return true;                                                       \
		return vector_##TYPE##_resize(v, capacity);                            \
	}                                                                          \
                                                                               \
	static bool vector_##TYPE##_init(VECTOR_T* v) {                            \
		/* if the vec is not initialized, allocate the buffer */               \
		if (v->buffer == NULL) {                                               \
			if (v->capacity == 0) return false;                                \
                                                                               \
			v->buffer = (TYPE_T*)calloc(v->capacity, sizeof(TYPE_T));          \
			if (v->buffer == NULL) return false; /* allocation failed */       \
		}                                                                      \
                                                                               \
		return true;                                                           \
	}                                                                          \
                                                                               \
	bool vector_##TYPE##_push_back(VECTOR_T* v, TYPE_T value) {                \
		if (!v || !vector_##TYPE##_init(v)) return false;                      \
                                                                               \
		if (v->size == v->capacity) {                                          \
			bool resized =                                                     \
			    vector_##TYPE##_ensure_capacity(v, v->capacity * 3 / 2);       \
			if (!resized) return false;                                        \
		}                                                                      \
                                                                               \
		v->buffer[v->size++] = value;                                          \
		return true;                                                           \
	}                                                                          \
                                                                               \
	bool vector_##TYPE##_pop_back(VECTOR_T* v, TYPE_T* out) {                  \
		if (!v || !out || v->size == 0) return false;                          \
                                                                               \
		TYPE_T value = v->buffer[v->size - 1];                                 \
		if (!vector_##TYPE##_shrink_one(v)) return false;                      \
                                                                               \
		*out = value;                                                          \
		return true;                                                           \
	}                                                                          \
                                                                               \
	bool vector_##TYPE##_insert(VECTOR_T* v, size_t idx, TYPE_T value) {       \
		if (!v || idx > v->size || !vector_##TYPE##_init(v)) {                 \
			return false;                                                      \
		}                                                                      \
                                                                               \
		if (v->size == v->capacity) {                                          \
			bool resized =                                                     \
			    vector_##TYPE##_ensure_capacity(v, v->capacity * 3 / 2);       \
			if (!resized) return false;                                        \
		}                                                                      \
                                                                               \
		/* Shift elements in |v->buffer| by one to fit the inserted value: */  \
		/* [ ... a_i, a_i+1, ..., a_n ] --> [ ... [] a_i, a_i+1, ..., a_n ] */ \
		memmove(v->buffer + idx + 1, v->buffer + idx,                          \
		        (v->size - idx) * sizeof(TYPE_T));                             \
                                                                               \
		v->buffer[idx] = value;                                                \
		v->size++;                                                             \
                                                                               \
		return true;                                                           \
	}                                                                          \
                                                                               \
	TYPE_T* vector_##TYPE##_get(const VECTOR_T* v, size_t i) {                 \
		if (!v || i >= v->size) return NULL;                                   \
		return &v->buffer[i];                                                  \
	}                                                                          \
                                                                               \
	bool vector_##TYPE##_remove(VECTOR_T* v, size_t i, TYPE_T* out) {          \
		if (!v || i >= v->size) return false;                                  \
                                                                               \
		TYPE_T value = v->buffer[i];                                           \
		if (out) *out = value;                                                 \
                                                                               \
		memmove(v->buffer + i, v->buffer + i + 1,                              \
		        (v->size - i - 1) * sizeof(TYPE_T));                           \
		if (!vector_##TYPE##_shrink_one(v)) return false;                      \
                                                                               \
		return true;                                                           \
	}                                                                          \
                                                                               \
	bool vector_##TYPE##_remove_item(VECTOR_T* v, TYPE_T value) {              \
		if (!v) return false;                                                  \
		return vector_##TYPE##_remove(v, vector_##TYPE##_index_of(v, value),   \
		                              NULL);                                   \
	}                                                                          \
                                                                               \
	bool vector_##TYPE##_clear(VECTOR_T* v) {                                  \
		if (!v || !vector_##TYPE##_resize(v, 4)) return false;                 \
                                                                               \
		v->size = 0;                                                           \
		return true;                                                           \
	}                                                                          \
                                                                               \
	size_t vector_##TYPE##_index_of(const VECTOR_T* v, TYPE_T value) {         \
		if (!v || v->utils.comp == NULL) return SIZE_MAX;                      \
                                                                               \
		for (size_t i = 0; i != v->size; i++) {                                \
			int order = v->utils.comp(&v->buffer[i], &value);                  \
			if (order == 0) return (ssize_t)i;                                 \
		}                                                                      \
                                                                               \
		return SIZE_MAX;                                                       \
	}                                                                          \
                                                                               \
	size_t vector_##TYPE##_size(const vector_##TYPE##_t* v) {                  \
		return v->size;                                                        \
	}                                                                          \
                                                                               \
	bool vector_##TYPE##_is_empty(const vector_##TYPE##_t* v) {                \
		return v->size == 0;                                                   \
	}                                                                          \
                                                                               \
	const TYPE_T* vector_##TYPE##_to_array(const vector_##TYPE##_t* v) {       \
		return (const TYPE_T*)v->buffer;                                       \
	}                                                                          \
                                                                               \
	bool vector_##TYPE##_sort(vector_##TYPE##_t* v) {                          \
		if (!v || !v->utils.comp) return false;                                \
                                                                               \
		qsort(v->buffer, v->size, sizeof(TYPE_T), v->utils.comp);              \
                                                                               \
		return true;                                                           \
	}                                                                          \
                                                                               \
	bool vector_##TYPE##_dup(const VECTOR_T* src, VECTOR_T* dst) {             \
		if (!src || !dst) return false;                                        \
                                                                               \
		*dst = vector_##TYPE##_create();                                       \
		if (!vector_##TYPE##_ensure_capacity(dst, src->capacity)) {            \
			return false;                                                      \
		}                                                                      \
                                                                               \
		memcpy(dst->buffer, src->buffer, src->size * sizeof(TYPE_T));          \
		dst->size = src->size;                                                 \
                                                                               \
		return true;                                                           \
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
