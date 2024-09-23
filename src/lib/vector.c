#include "lib/vector.h"

#include <float.h>
#include <stdlib.h>
#include <string.h>

const size_t MIN_CAPACITY = 4;

#define IMPL_VECTOR(VTYPE, TYPE, TNAME, TDEF)                                                           \
	bool vector_##TNAME##_resize(VTYPE* v, size_t capacity) {                                           \
		TYPE* newBuffer = realloc(v->buffer, capacity * sizeof(TYPE));                                  \
		if (newBuffer == NULL) return false;                                                            \
                                                                                                        \
		v->buffer = newBuffer;                                                                          \
		v->capacity = capacity;                                                                         \
		return true;                                                                                    \
	}                                                                                                   \
                                                                                                        \
	bool vector_##TNAME##_shrink_one(VTYPE* v) {                                                        \
		if ((v->size - 1) > MIN_CAPACITY && (v->size - 1) <= v->capacity * 4 / 9) {                     \
			bool resized = vector_##TNAME##_resize(v, v->capacity * 2 / 3);                             \
			if (!resized) return false;                                                                 \
		}                                                                                               \
                                                                                                        \
		v->size--;                                                                                      \
		return true;                                                                                    \
	}                                                                                                   \
                                                                                                        \
	VTYPE vector_##TNAME##_create(void) { return vector_##TNAME##_create_with_capacity(MIN_CAPACITY); } \
                                                                                                        \
	VTYPE vector_##TNAME##_create_with_capacity(size_t capacity) {                                      \
		/* allocate later, when a push occurs */                                                        \
		VTYPE v = {.buffer = NULL, .size = 0, .capacity = capacity};                                    \
		return v;                                                                                       \
	}                                                                                                   \
                                                                                                        \
	void vector_##TNAME##_destroy(VTYPE* v) {                                                           \
		if (v->buffer != NULL) free(v->buffer);                                                         \
                                                                                                        \
		v->buffer = NULL;                                                                               \
		v->size = 0;                                                                                    \
		v->capacity = 0;                                                                                \
	}                                                                                                   \
                                                                                                        \
	bool vector_##TNAME##_ensure_capacity(VTYPE* v, size_t capacity) {                                  \
		if (v->capacity >= capacity) return true;                                                       \
		return vector_##TNAME##_resize(v, capacity);                                                    \
	}                                                                                                   \
                                                                                                        \
	bool vector_##TNAME##_push_back(VTYPE* v, TYPE value) {                                             \
		/* if the vec is not initialized, allocate the buffer */                                        \
		if (v->buffer == NULL) {                                                                        \
			if (v->capacity == 0) return false;                                                         \
                                                                                                        \
			v->buffer = calloc(v->capacity, sizeof(TYPE));                                              \
			if (v->buffer == NULL) return false; /* allocation failed */                                \
		}                                                                                               \
                                                                                                        \
		if (v->size == v->capacity) {                                                                   \
			bool resized = vector_##TNAME##_ensure_capacity(v, v->capacity * 3 / 2);                    \
			if (!resized) return false;                                                                 \
		}                                                                                               \
                                                                                                        \
		v->buffer[v->size++] = value;                                                                   \
		return true;                                                                                    \
	}                                                                                                   \
                                                                                                        \
	TYPE vector_##TNAME##_pop_back(VTYPE* v) {                                                          \
		if (v->size == 0) return TDEF;                                                                  \
                                                                                                        \
		TYPE value = v->buffer[v->size - 1];                                                            \
		if (!vector_##TNAME##_shrink_one(v)) return TDEF;                                               \
                                                                                                        \
		return value;                                                                                   \
	}                                                                                                   \
                                                                                                        \
	TYPE vector_##TNAME##_get(VTYPE* v, size_t index) {                                                 \
		if (index >= v->size) return TDEF;                                                              \
		return v->buffer[index];                                                                        \
	}                                                                                                   \
                                                                                                        \
	TYPE vector_##TNAME##_remove(VTYPE* v, size_t index) {                                              \
		if (index >= v->size) return TDEF;                                                              \
                                                                                                        \
		TYPE value = v->buffer[index];                                                                  \
		memmove(v->buffer + index, v->buffer + index + 1, (v->size - index - 1) * sizeof(TYPE));        \
                                                                                                        \
		if (!vector_##TNAME##_shrink_one(v)) return TDEF;                                               \
                                                                                                        \
		return value;                                                                                   \
	}                                                                                                   \
                                                                                                        \
	TYPE vector_##TNAME##_remove_item(VTYPE* v, TYPE value) {                                           \
		return vector_##TNAME##_remove(v, vector_##TNAME##_index_of(v, value));                         \
	}                                                                                                   \
                                                                                                        \
	bool vector_##TNAME##_clear(VTYPE* v) {                                                             \
		if (!vector_##TNAME##_resize(v, 4)) return false;                                               \
                                                                                                        \
		v->size = 0;                                                                                    \
		return true;                                                                                    \
	}                                                                                                   \
                                                                                                        \
	size_t vector_##TNAME##_index_of(VTYPE* v, TYPE value) {                                            \
		for (size_t i = 0; i != v->size; i++) {                                                         \
			if (v->buffer[i] == value) return (ssize_t)i;                                               \
		}                                                                                               \
                                                                                                        \
		return SIZE_MAX;                                                                                \
	}                                                                                                   \
                                                                                                        \
	size_t vector_##TNAME##_size(VTYPE* v) { return v->size; }                                          \
                                                                                                        \
	bool vector_##TNAME##_is_empty(VTYPE* v) { return v->size == 0; }                                   \
                                                                                                        \
	TYPE* vector_##TNAME##_to_array(VTYPE* v) { return v->buffer; }                                     \
                                                                                                        \
	bool vector_##TNAME##_swap(vector_##TNAME##_t* v, size_t i1, size_t i2) {                           \
		if (i1 >= v->size || i2 >= v->size) return false;                                               \
                                                                                                        \
		TYPE temp = v->buffer[i1];                                                                      \
		v->buffer[i1] = v->buffer[i2];                                                                  \
		v->buffer[i2] = temp;                                                                           \
		return true;                                                                                    \
	}

IMPL_VECTOR(vector_i64_t, int64_t, i64, INT64_MAX)
IMPL_VECTOR(vector_i32_t, int32_t, i32, INT32_MAX)
IMPL_VECTOR(vector_i16_t, int16_t, i16, INT16_MAX)
IMPL_VECTOR(vector_i8_t, int8_t, i8, INT8_MAX)
IMPL_VECTOR(vector_u64_t, uint64_t, u64, UINT64_MAX)
IMPL_VECTOR(vector_u32_t, uint32_t, u32, UINT32_MAX)
IMPL_VECTOR(vector_u16_t, uint16_t, u16, UINT16_MAX)
IMPL_VECTOR(vector_u8_t, uint8_t, u8, UINT8_MAX)
IMPL_VECTOR(vector_flt_t, float, flt, FLT_MAX)
IMPL_VECTOR(vector_dbl_t, double, dbl, DBL_MAX)
IMPL_VECTOR(vector_ptr_t, void*, ptr, NULL)
